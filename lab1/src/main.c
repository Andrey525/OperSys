#include "fileman.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc == 2) {
        if (strcmp(argv[1], "--printdir") == 0) {
            print_dir(".");
        }
    } else if (argc == 3) {
        if (strcmp(argv[1], "--create") == 0) {
            create_file(argv[2]);
        } else if (strcmp(argv[1], "--showcontent") == 0) {
            view_file_content(argv[2]);
        } else if (strcmp(argv[1], "--remove") == 0) {
            remove_file(argv[2]);
        } else if (strcmp(argv[1], "--printdir") == 0) {
            print_dir(argv[2]);
        }
    } else if (argc == 4) {
        if (strcmp(argv[1], "--cutpaste") == 0) {
            cut_paste_file(argv[2], argv[3]);
        } else if (strcmp(argv[1], "--copypaste") == 0) {
            copy_paste_file(argv[2], argv[3]);
        } else if (strcmp(argv[1], "--linkcreate") == 0) {
            link_create(argv[2], argv[3]);
        }
    }
    return 0;
}