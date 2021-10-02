#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int create_file(const char *pathToFile) {
    int fd;
    fd = open(pathToFile, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        perror("File creation error");
        return -1;
    }
    if (close(fd) == -1) {
        perror("File closing error");
        return -1;
    }
    return 0;
}

int view_file_content(const char *pathToFile) {
    int fd;
    fd = open(pathToFile, O_RDONLY);
    if (fd == -1) {
        perror("Сontent view error");
        return -1;
    }

    ssize_t ret;
    char ch;

    while ((ret = read(fd, &ch, 1)) > 0) {
        putchar(ch);
    }

    if (ret == -1) {
        perror("Error");
        return -1;
    }

    if (close(fd) == -1) {
        perror("File closing error");
        return -1;
    }
    return 0;
}

int remove_file(const char *pathToFile) {

    if (unlink(pathToFile) == -1) {
        perror("File deletion error");
        return -1;
    }
    return 0;
}

int cut_paste_file(const char *srcPathToFile, const char *dstPathToFile) {
    if (rename(srcPathToFile, dstPathToFile) == -1) {
        perror("Cut/paste error");
        return -1;
    }
    return 0;
}

int copy_paste_file(const char *srcPathToFile, const char *dstPathToFile) {
    int fdsrc, fddst;
    fdsrc = open(srcPathToFile, O_RDONLY);
    if (fdsrc == -1) {
        perror("Copy/paste error");
        return -1;
    }
    fddst = open(dstPathToFile, O_CREAT | O_EXCL | O_WRONLY | O_APPEND,
                 S_IRUSR | S_IWUSR);
    if (fddst == -1) {
        perror("Copy/paste error");
        return -1;
    }

    ssize_t ret, nr;
    char ch;

    while ((ret = read(fdsrc, &ch, 1)) > 0) {
        nr = write(fddst, &ch, 1);
        if (nr == -1) {
            perror("Copy/paste error");
            return -1;
        }
    }

    if (ret == -1) {
        perror("Copy/paste error");
        return -1;
    }
    if (close(fdsrc) == -1) {
        perror("File closing error");
        return -1;
    }
    if (close(fddst) == -1) {
        perror("File closing error");
        return -1;
    }
    return 0;
}

int link_create(const char *oldPathToFile, const char *newPathToFile) {
    if (link(oldPathToFile, newPathToFile) == -1) {
        perror("Link create");
        return -1;
    }
    return 0;
}

int print_dir(const char *path) {

    struct dirent *entry;
    DIR *dir;
    dir = opendir(path);
    if (dir == NULL) {
        perror("Print dir error");
        return -1;
    }
    errno = 0;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(".", entry->d_name) != 0 &&
            strcmp("..", entry->d_name) != 0) {
            printf("%s ", entry->d_name);
        }
    }
    printf("\n");
    if (errno && !entry) {
        perror("Print dir error");
    }
    closedir(dir);

    return 0;
}