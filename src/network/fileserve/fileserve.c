#include "../mx_socket.h"
#include <dirent.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdatomic.h>
#include <sys/ioctl.h>

static constexpr size_t PORT_MAX = 4951;
static constexpr size_t PORT_MIN = 1024;
static constexpr size_t BUFFER_SIZE = 4096;


ssize_t write_all(int fd, const void *buf, size_t bytes) {
    const char *ptr = buf;
    size_t left = bytes;
    while (left > 0) {
        ssize_t written = write(fd, ptr, left);
        if (written == -1) {
            if (errno == EINTR) {
                continue;
            }
            return -1;
        }
        left -= (size_t)written;
        ptr += written;
    }
    return (ssize_t)bytes;
}

static void send_error(MXSocket *sock, const char *message) {
    size_t len = strlen(message);
    if(mx_socket_write_all(sock, message, len) != (ssize_t)len) {
        fprintf(stderr, "fileserve: Error could not send data.\n");
        return;
    }
}

void list_directory(MXSocket *sock) {
    DIR *d = opendir(".");
    if (d == nullptr) {
        fprintf(stderr, "fileserve: Error could not open directory.\n");
        const char *response = "error: 100\r\n";
        size_t rlen = strlen(response);
        if (mx_socket_write_all(sock, response, rlen) != (ssize_t)rlen) {
            fprintf(stderr, "fileserve: Error could not send complete reponse.\n");
        }
        return;
    }
    struct dirent *e;
    while ((e = readdir(d)) != nullptr) {
        if (strcmp(e->d_name, "..") == 0 || strcmp(e->d_name, ".") == 0)
            continue;
        if (e->d_type != DT_DIR) {
            char buffer[PATH_MAX + 25];
            snprintf(buffer, PATH_MAX + 25, "%s\n", e->d_name);
            size_t len = strlen(buffer);
            if (mx_socket_write_all(sock, buffer, len) != (ssize_t)len) {
                fprintf(stderr, "fileserve: could not write data.\n");
                return;
            }
        }
    }
    closedir(d);
    const char *end_list = "\r\n";
    size_t end_len = strlen(end_list);
    if (mx_socket_write_all(sock, end_list, end_len) != (ssize_t)end_len) {
        fprintf(stderr, "fileserve: could not write data.\n");
        return;
    }
}

void send_file(MXSocket *sock, const char *filename) {
    if (strstr(filename, "/") != nullptr || strstr(filename, "\\") != nullptr || strstr(filename, "..") != nullptr) {
        fprintf(stderr, "fileserve: Error invalid path.");
        const char *response = "error: 101\r\n";
        size_t res_len = strlen(response);
        if (mx_socket_write_all(sock, response, res_len) != (ssize_t)res_len) {
            fprintf(stderr, "fileserve: Error could not write data.\n");
            return;
        }
        return;
    }

    int ofd = openat(AT_FDCWD, filename, O_RDONLY);
    if (ofd == -1) {
        perror("open");
        fprintf(stderr, "Could not open file: %s\n", filename);
        send_error(sock, "error: 103\r\n");
        return;
    }
    struct stat dst;
    if (fstat(ofd, &dst) == 0) {
        char buffer[BUFFER_SIZE] = {};
        snprintf(buffer, BUFFER_SIZE, "Content-Length: %zu\n", (size_t)dst.st_size);
        size_t blen = strlen(buffer);
        if (mx_socket_write_all(sock, buffer, blen) != (ssize_t)blen) {
            fprintf(stderr, "fileserve: Error could not write data.\n");
            close(ofd);
            return;
        }
    } else {
        fprintf(stderr, "fileserve: Error could not stat file.\n");
        close(ofd);
        return;
    }
    sendfile(sock->sockfd, ofd, nullptr, (size_t)dst.st_size);
    close(ofd);
}

atomic_bool active_loop = false;

void *process_input(void *data) {
    char buffer[BUFFER_SIZE] = {};
    MXSocket *sock = (MXSocket *)data;
    while (atomic_load(&active_loop)) {
        memset(buffer, 0, sizeof(buffer));
        ssize_t bytes = 0;
        while ((bytes = mx_socket_read(sock, buffer, sizeof(buffer) - 1, 0)) > 0) {
            buffer[bytes] = 0;
            printf("fileserve: command: %s", buffer);
            const char *cmd = strstr(buffer, "ls");
            if (cmd != nullptr) {
                list_directory(sock);
                continue;
            }
            char *get_file = strstr(buffer, "get: ");
            if (get_file != nullptr) {
                get_file += 5;
                char *nb = strstr(get_file, "\r\n");
                if (nb != nullptr) {
                    *nb = '\0';
                    printf("fileserve: sending file: %s\n", get_file);
                    send_file(sock, get_file);
                    continue;
                }
                send_error(sock, "error: 102\r\n");
            }
            const char *exit_cmd = strstr(buffer, "exit:");
            if (exit_cmd != nullptr) {
                printf("Exiting thread socket[%d]\n", sock->sockfd);
                mx_socket_close(sock);
                free(sock);
                return nullptr;
            }
            printf("fileserve: invalid command.\n");

            continue;
        }
        if (bytes <= 0)
            break;
    }
    printf("Exiting thread socket[%d]", sock->sockfd);
    mx_socket_close(sock);
    free(sock);
    return nullptr;
}

void listen_signal(int) {
    atomic_store(&active_loop, false);
}

static void listen_server(const char *port) {
    MXSocket sock;
    if (mx_socket_listen(&sock, port, 5)) {
        printf("file_serve: Listening on port %s\n", port);
        atomic_store(&active_loop, true);
        while (atomic_load(&active_loop)) {
            MXSocket *new_socket = malloc(sizeof(MXSocket));
            if (new_socket == nullptr) {
                fprintf(stderr, "fileserve: Memory exhausted.\n");
                mx_socket_close(&sock);
                return;
            }
            if (!mx_socket_init(new_socket)) {
                fprintf(stderr, "fileserve: Error init socket struct.\n");
                mx_socket_close(&sock);
                free(new_socket);
                return;
            }
            if (mx_socket_accept(&sock, new_socket)) {
                pthread_t id;
                if (pthread_create(&id, nullptr, process_input, new_socket) == 0) {
                    pthread_detach(id);
                } else {
                    mx_socket_close(new_socket);
                    free(new_socket);
                    fprintf(stderr, "fileserve: Error spawning new thread...\n");
                    atomic_store(&active_loop, false);
                    mx_socket_close(&sock);
                    return;
                }
            } else {
                free(new_socket);
                if (errno == EINTR) {
                    continue;
                }
            }
        }
    } else {
        fprintf(stderr, "file_serve: Error on listen..\n");
        return;
    }
    printf("fileserve: Exiting...\n");
    mx_socket_close(&sock);
}

static int get_terminal_width() {
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1) {
        return 80;
    }
    return w.ws_col;
}

static void print_progress(int *prev, size_t length, size_t progress) {
    if (length == 0) {
        printf("\r\033[2K[ Downloading ] - 0%%\n");
        return;
    }
    if (progress > length) {
        progress = length;
    }
    int term_width = get_terminal_width();
    int bar_width = term_width - 12;
    double ratio = (double)progress / (double)length;
    int num_equals = (int)(ratio * bar_width);
    int percent = (int)(ratio * 100.0);

    if(*prev == percent)
        return;
    *prev = percent;
    if (bar_width < 10)
        bar_width = 10;
    printf("\r\033[2K[");
    for (int j = 0; j < bar_width; ++j) {
        if (j < num_equals) {
            putchar('=');
        } else {
            putchar(' ');
        }
    }
    printf("] - %d%%", percent);

    fflush(stdout);
}

static void connect_client(const char *host, const char *port) {
    MXSocket sock;
    atomic_store(&active_loop, true);
    if (mx_socket_connect(&sock, host, port, SOCK_STREAM)) {
        while (atomic_load(&active_loop)) {
            char input[BUFFER_SIZE] = {};
            char buffer[BUFFER_SIZE] = {};
            printf("fileserve> ");
            if (fgets(input, BUFFER_SIZE - 3, stdin)) {
                size_t slen = strlen(input);
                input[slen - 1] = '\0';
                if (slen + 2 < BUFFER_SIZE) {
                    strncat(input, "\r\n", BUFFER_SIZE - 1);
                }
                slen = strlen(input);
                if (mx_socket_write_all(&sock, input, slen) != (ssize_t)slen) {
                    fprintf(stderr, "fserve: Error could not write data.\n");
                    break;
                }
                if (strncmp(input, "exit:", 5) == 0) {
                    break;
                }

                memset(buffer, 0, sizeof(buffer));

                if(strncmp(input, "help", 4) == 0) {
                    printf("program commands:\nhelp\t[this message]\nls\t[list files]\nget: <filename>\t[get file]\nexit:\t[exit program.]\n");
                    continue;
                } else if (strncmp(input, "ls", 2) == 0) {
                    while (1) {
                        ssize_t bytes = mx_socket_read(&sock, buffer, sizeof(buffer) - 1, 0);
                        if (bytes <= 0) {
                            break;
                        }
                        buffer[bytes] = '\0';
                        printf("%s", buffer);
                        if (strstr(buffer, "\r\n") != nullptr) {
                            break;
                        }
                        memset(buffer, 0, sizeof(buffer));
                    }
                } else if (strncmp(input, "get: ", 5) == 0) {
                    ssize_t bytes = mx_socket_read(&sock, buffer, sizeof(buffer) - 1, 0);
                    if (bytes <= 0)
                        continue;
                    buffer[bytes] = '\0';
                    if (strncmp(buffer, "error:", 6) == 0) {
                        fprintf(stderr, "fileserve: Server returned  %s\n", buffer);
                        continue;
                    }
                    char *cl_ptr = strstr(buffer, "Content-Length: ");
                    if (cl_ptr != nullptr) {
                        cl_ptr += 16;
                        size_t file_size = (size_t)strtoul(cl_ptr, nullptr, 10);
                        char filename[PATH_MAX] = {};
                        char fmt_string[32] = {};
                        snprintf(fmt_string, sizeof(fmt_string), "get: %%%zus", (size_t)PATH_MAX - 1);
                        int fd = -1;
                        if (sscanf(input, fmt_string, filename) == 1) {
                            fd = openat(AT_FDCWD, filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                            if (fd == -1) {
                                perror("openat");
                                continue;
                            }
                        } else {
                            fprintf(stderr, "fileserve: Error on interpreting get command.\n");
                            continue;
                        }

                        char *data_start = strchr(cl_ptr, '\n');
                        size_t bytes_written = 0;
                        if (data_start != nullptr) {
                            data_start++;
                            size_t header_len = (size_t)(data_start - buffer);
                            size_t init_data_len = (size_t)bytes - header_len;
                            if (init_data_len > 0) {
                                if (write_all(fd, data_start, init_data_len) != (ssize_t)init_data_len) {
                                    fprintf(stderr, "fileserve: Error writing initial data.\n");
                                } else {
                                    bytes_written += init_data_len;
                                }
                            }
                        }
                        int prev = 0;
                        print_progress(&prev, file_size, bytes_written);
                        while (bytes_written < file_size) {
                            memset(buffer, 0, sizeof(buffer));
                            size_t to_read = BUFFER_SIZE;
                            if (file_size - bytes_written < BUFFER_SIZE) {
                                to_read = file_size - bytes_written;
                            }
                            bytes = mx_socket_read(&sock, buffer, to_read, 0);
                            if (bytes <= 0) {
                                fprintf(stderr, "fileserve: Error writing to file.\n");
                                break;
                            }

                            if (write_all(fd, buffer, (size_t)bytes) != (ssize_t)bytes) {
                                fprintf(stderr, "fileserve: Error writing to file.\n");
                                break;
                            }
                            bytes_written += (size_t)bytes;
                            print_progress(&prev, file_size, bytes_written);
                        }
                        print_progress(&prev, file_size, bytes_written);
                        printf("\nfileserve:  Saved %zu bytes to %s\n", bytes_written, filename);
                        close(fd);
                    }
                } else {
                    printf("fileserve: invalid command.\n");
                }
            } else {
                break;
            }
        }
        mx_socket_close(&sock);
    } else {
        fprintf(stderr, "fileserve: Error could not connect");
        perror("connect");
    }
}

int main(int argc, char **argv) {
    signal(SIGPIPE, SIG_IGN);
    struct sigaction sa;
    sa.sa_handler = listen_signal;
    sa.sa_flags = 0;
    if (sigaction(SIGINT, &sa, nullptr) == -1) {
        fprintf(stderr, "fileserve: Could not install signal handler.\n");
        return EXIT_FAILURE;
    }
    if (argc == 2) {
        char *end = nullptr;
        unsigned int port = (unsigned int)strtoul(argv[1], &end, 10);
        if (end == argv[1] || (end != nullptr && *end != '\0')) {
            fprintf(stderr, "fileserve: Error use:\n%s <port>\n", argv[0]);
            return EXIT_FAILURE;
        }
        if (port < PORT_MIN || port > PORT_MAX) {
            fprintf(stderr, "fileserve: Error port out of range.\nUse: %zu - %zu\n (suggested: 3000)", PORT_MIN, PORT_MAX);
            return EXIT_FAILURE;
        }
        listen_server(argv[1]);
    } else if (argc == 3) {
        char *end = nullptr;
        unsigned int port = (unsigned int)strtoul(argv[2], &end, 10);
        if (end == argv[2] || (end != nullptr && *end != '\0')) {
            fprintf(stderr, "fileserve: Error use:\n%s <hostname> <port>\n", argv[0]);
            return EXIT_FAILURE;
        }
        if (port < PORT_MIN || port > PORT_MAX) {
            fprintf(stderr, "fileserve: Error port out of range.\nUse: %zu-%zu (suggested: 3000)\n", PORT_MIN, PORT_MAX);
            return EXIT_FAILURE;
        }
        printf("Connecting...\n");
        connect_client(argv[1], argv[2]);
    }
    printf("fileserve: Exiting.\n");
    return EXIT_SUCCESS;
}
