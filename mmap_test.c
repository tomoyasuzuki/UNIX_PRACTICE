#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
    int fd;
    struct stat sb;
    void *head;
    char *testb;
    int flag;

    // asumption: argv[1]に"This is a test file."と書き込んだテキストファイルを渡す

    flag = atoi(argv[2]);
    printf("flag=%d\n", flag);
    
    if ((fd = open(argv[1], flag == 1 ? O_RDONLY : O_RDWR)) == 0) {
        fprintf(stderr, "fail to open file\n");
        exit(1);
    }

    fstat(fd, &sb);
    head = mmap(NULL, sb.st_size, flag == 1 ? PROT_READ : PROT_WRITE, MAP_SHARED, fd, 0);

    testb = "XXXXX";
    if (write(fd, testb, 5) < 0) {
        fprintf(stdout, "write failed.\n");
        if (mprotect(head, sb.st_size, PROT_WRITE) < 0) {
            // flag = 1 の場合はここにくるのが正しい
            // mprotect は権限が設定されていないメモリに権限を設定するものなので、
            // mmapなどで既にPROT_READに設定されているメモリにPROT_WRITEを設定する
            // などの使い方はできない
            fprintf(stderr, "previlige change failed.\n");
            exit(0);
        }
    } else {
        *(char *)(head + 1) = 'g';
        printf("changed: %s\n", (char *)head);
        // changed: XgXXXis a test file.
    }
    munmap(head, sb.st_size);
    close(fd);
    exit(0);
}