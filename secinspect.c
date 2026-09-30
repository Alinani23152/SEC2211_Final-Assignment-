#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>

#define BUFFER_SIZE 256

int main(int argc, char *argv[])
{
    int fd;
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;
    ssize_t bytes_written;
    off_t offset;
    struct stat file_info;

    /* Check that a filename was provided */
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return EXIT_FAILURE;
    }

    printf("=== secinspect ===\n");
    printf("File: %s\n", argv[1]);

    /* 1. OPEN THE FILE */
    fd = open(argv[1], O_RDWR);

    if (fd == -1) {
        fprintf(stderr, "open() failed: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    printf("\n[OPEN]\n");
    printf("File descriptor: %d\n", fd);

    /* 2. GET FILE METADATA USING fstat() */
    if (fstat(fd, &file_info) == -1) {
        fprintf(stderr, "fstat() failed: %s\n", strerror(errno));
        close(fd);
        return EXIT_FAILURE;
    }

    printf("\n[FILE METADATA]\n");
    printf("File size: %ld bytes\n", (long)file_info.st_size);
    printf("Permissions: %o\n", file_info.st_mode & 0777);
    printf("Inode: %ld\n", (long)file_info.st_ino);

    /* 3. READ FROM THE FILE */
    bytes_read = read(fd, buffer, BUFFER_SIZE - 1);

    if (bytes_read == -1) {
        fprintf(stderr, "read() failed: %s\n", strerror(errno));
        close(fd);
        return EXIT_FAILURE;
    }

    buffer[bytes_read] = '\0';

    printf("\n[READ]\n");
    printf("Bytes read: %ld\n", (long)bytes_read);
    printf("Data read:\n%s\n", buffer);

    /* 4. CHANGE FILE OFFSET USING lseek() */
    offset = lseek(fd, 0, SEEK_SET);

    if (offset == (off_t)-1) {
        fprintf(stderr, "lseek() failed: %s\n", strerror(errno));
        close(fd);
        return EXIT_FAILURE;
    }

    printf("\n[LSEEK]\n");
    printf("File offset changed to: %ld\n", (long)offset);

    /* 5. MOVE TO END OF FILE */
    offset = lseek(fd, 0, SEEK_END);

    if (offset == (off_t)-1) {
        fprintf(stderr, "lseek() to end failed: %s\n", strerror(errno));
        close(fd);
        return EXIT_FAILURE;
    }

    /* 6. WRITE TO THE FILE */
    const char *message = "\n[secinspect] write() demonstration\n";

    bytes_written = write(fd, message, strlen(message));

    if (bytes_written == -1) {
        fprintf(stderr, "write() failed: %s\n", strerror(errno));
        close(fd);
        return EXIT_FAILURE;
    }

    printf("\n[WRITE]\n");
    printf("Bytes written: %ld\n", (long)bytes_written);
printf("\nProcess PID: %d\n", getpid());
printf("Sleeping for 30 seconds so file descriptors can be inspected...\n");
sleep(30);
printf("\nProcess PID: %d\n", getpid());
printf("Sleeping for 30 seconds...\n");
sleep(30);

    /* 7. CLOSE THE FILE */
    if (close(fd) == -1) {
        fprintf(stderr, "close() failed: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    printf("\n[CLOSE]\n");
    printf("File descriptor closed successfully.\n");

    return EXIT_SUCCESS;
}
