#ifndef _ISP_UTILS_H_INC_
#define _ISP_UTILS_H_INC_

#define isp_close(fd)                                                          \
    do {                                                                       \
        if (fd >= 0) {                                                         \
            close(fd);                                                         \
            fd = -1;                                                           \
        }                                                                      \
    } while (0)

#endif /* _ISP_UTILS_H_INC_ */
