#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <limits.h>

enum dir_type {
    type_file = 0,
    type_dir
};

struct dir_node {
    char filename[PATH_MAX];
    enum dir_type type;
    struct dir_node *next;
};

static struct dir_node *merge(struct dir_node *left, struct dir_node *right) {
    if (!left)
        return right;
    if (!right)
        return left;

    struct dir_node dummy = {};
    struct dir_node *tail = &dummy;

    while (left && right) {
        if (strcmp(left->filename, right->filename) <= 0) {
            tail->next = left;
            left = left->next;
        } else {
            tail->next = right;
            right = right->next;
        }
        tail = tail->next;
    }

    tail->next = left ? left : right;
    return dummy.next;
}

static struct dir_node *get_middle(struct dir_node *head) {
    if (!head || !head->next)
        return head;

    struct dir_node *slow = head;
    struct dir_node *fast = head->next;

    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }
    return slow;
}

static struct dir_node *merge_sort(struct dir_node *head) {
    if (!head || !head->next)
        return head;

    struct dir_node *mid = get_middle(head);
    struct dir_node *right_half = mid->next;
    mid->next = nullptr;

    struct dir_node *left = merge_sort(head);
    struct dir_node *right = merge_sort(right_half);

    return merge(left, right);
}

static struct dir_node *create_node(const char *file_name, enum dir_type t) {
    if (file_name == nullptr) {
        fprintf(stderr, "Error: invalid filename.\n");
        return nullptr;
    }
    struct dir_node *n;
    n = calloc(1, sizeof(struct dir_node));
    if (n == nullptr) {
        fprintf(stderr, "Error: Memory exhausted.\n");
        return nullptr;
    }
    n->type = t;
    strncpy(n->filename, file_name, PATH_MAX - 1);
    n->filename[PATH_MAX - 1] = '\0';
    n->next = nullptr;
    return n;
}

static void cleanup_node(struct dir_node *n) {
    struct dir_node *rls = n;
    while (rls != nullptr) {
        struct dir_node *next = rls->next;
        free(rls);
        rls = next;
    }
}

static void print_node(struct dir_node *n) {
    struct dir_node *rls = n;
    while (rls != nullptr) {
        if (rls->type == type_file)
            printf("%s\n", rls->filename);
        rls = rls->next;
    }
}

static int dir_list(const char *src, struct dir_node **n) {
    DIR *dir = opendir(src);
    if (dir == nullptr) {
        fprintf(stderr, "Error reading from dir: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }
    struct dirent *d = nullptr;
    struct dir_node **tail = n;
    while ((d = readdir(dir)) != nullptr) {
        if (strcmp(d->d_name, "..") == 0 || strcmp(d->d_name, ".") == 0)
            continue;
        enum dir_type type = type_file;
        if (d->d_type == DT_DIR) {
            type = type_dir;
        } else if (d->d_type == DT_UNKNOWN) {
            struct stat st;
            if (fstatat(dirfd(dir), d->d_name, &st, 0) == 0) {
                if (S_ISDIR(st.st_mode)) {
                    type = type_dir;
                }
            }
        }
        char buffer[PATH_MAX];
        if (strcmp(src, ".") == 0)
            snprintf(buffer, PATH_MAX - 1, "%s", d->d_name);
        else
            snprintf(buffer, PATH_MAX - 1, "%s/%s", src, d->d_name);

        if (type == type_dir) {
            struct dir_node *temp = create_node(buffer, type);
            *tail = temp;
            tail = &(*tail)->next;
            dir_list(buffer, tail);
            while (*tail != nullptr)
                tail = &(*tail)->next;
            continue;
        } else {
            struct dir_node *temp = create_node(buffer, type);
            if (temp == nullptr) {
                fprintf(stderr, "Error exiting..\n");
                cleanup_node(*n);
                closedir(dir);
                return EXIT_FAILURE;
            }
            *tail = temp;
            tail = &(*tail)->next;
        }
    }
    closedir(dir);
    return EXIT_SUCCESS;
}

int main(int argc, char *argv[argc + 1]) {
    struct dir_node *dirs = nullptr;
    if (dir_list((argc <= 1) ? "." : argv[1], &dirs) == EXIT_FAILURE) {
        printf("failure.\n");
        return EXIT_FAILURE;
    }
    dirs = merge_sort(dirs);
    print_node(dirs);
    cleanup_node(dirs);
    return EXIT_SUCCESS;
}
