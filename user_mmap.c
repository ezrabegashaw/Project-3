#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define PROC_PATH "/proc/mydir/myinfo"
#define PAGE_SIZE 4096

int main(int argc, char *argv[])
{
    const char *name = (argc > 1) ? argv[1] : "Student";

    int fd = open(PROC_PATH, O_RDWR);
    if (fd < 0) {
        fprintf(stderr, "open %s failed: %s\n", PROC_PATH, strerror(errno));
        return 1;
    }
    printf("open successfully by %s\n", name);

    unsigned char *p_map = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (p_map == MAP_FAILED) {
        fprintf(stderr, "mmap failed: %s\n", strerror(errno));
        close(fd);
        return 1;
    }

    for (int i = 0; i < 12; ++i) {
        printf("%d\n", p_map[i]);
    }

    // optional tag to prove write works
    snprintf((char *)p_map + 128, 64, "Printed by %s", name);

    munmap(p_map, PAGE_SIZE);
    close(fd);

    printf("Printed by %s\n", name);
    return 0;
}
