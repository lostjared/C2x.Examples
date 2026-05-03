#include <errno.h>
#include <fcntl.h>
#include <linux/videodev2.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define CLR_HDR "\x1b[1;34m"
#define CLR_FMT "\x1b[1;32m"
#define CLR_DIM "\x1b[2;37m"
#define CLR_FPS "\x1b[1;33m"
#define CLR_OFF "\x1b[0m"

void list_device(int device_idx) {
    char dev_path[64];
    snprintf(dev_path, sizeof(dev_path), "/dev/video%d", device_idx);

    int fd = open(dev_path, O_RDWR);
    if (fd < 0) {
        fprintf(stderr, "Error opening %s: %s\n", dev_path, strerror(errno));
        return;
    }

    struct v4l2_capability cap;
    if (ioctl(fd, VIDIOC_QUERYCAP, &cap) == 0) {
        printf("\n%s[ Device: %s ]%s\n", CLR_HDR, dev_path, CLR_OFF);
        printf("%sCard: %s | Driver: %s%s\n", CLR_DIM, cap.card, cap.driver, CLR_OFF);
        printf("----------------------------------------------------------\n");
    }

    struct v4l2_fmtdesc fmt = {.type = V4L2_BUF_TYPE_VIDEO_CAPTURE, .index = 0};

    while (ioctl(fd, VIDIOC_ENUM_FMT, &fmt) == 0) {
        char fourcc[5];
        memcpy(fourcc, &fmt.pixelformat, 4);
        fourcc[4] = '\0';

        printf("\n %s[%s]%s %s\n", CLR_FMT, fourcc, CLR_OFF, fmt.description);

        struct v4l2_frmsizeenum fsize = {.pixel_format = fmt.pixelformat, .index = 0};
        while (ioctl(fd, VIDIOC_ENUM_FRAMESIZES, &fsize) == 0) {
            if (fsize.type == V4L2_FRMSIZE_TYPE_DISCRETE) {
                printf("  %s├─%s %4dx%-4d @ ", CLR_DIM, CLR_OFF, fsize.discrete.width, fsize.discrete.height);

                struct v4l2_frmivalenum fival = {
                    .pixel_format = fmt.pixelformat,
                    .width = fsize.discrete.width,
                    .height = fsize.discrete.height,
                    .index = 0};

                bool first = true;
                while (ioctl(fd, VIDIOC_ENUM_FRAMEINTERVALS, &fival) == 0) {
                    if (fival.type == V4L2_FRMIVAL_TYPE_DISCRETE) {
                        double fps = (double)fival.discrete.denominator / fival.discrete.numerator;
                        printf("%s%g%s", first ? "" : ", ", fps, CLR_FPS);
                        first = false;
                    }
                    fival.index++;
                }
                printf("%s fps\n", CLR_OFF);

            } else if (fsize.type == V4L2_FRMSIZE_TYPE_STEPWISE) {
                printf("  %s└─%s %dx%d to %dx%d (step %dx%d)\n",
                       CLR_DIM, CLR_OFF,
                       fsize.stepwise.min_width, fsize.stepwise.min_height,
                       fsize.stepwise.max_width, fsize.stepwise.max_height,
                       fsize.stepwise.step_width, fsize.stepwise.step_height);
                break;
            }
            fsize.index++;
        }
        fmt.index++;
    }

    printf("\n");
    close(fd);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <device_index>\n", argv[0]);
        return EXIT_FAILURE;
    }
    list_device(atoi(argv[1]));
    return EXIT_SUCCESS;
}
