#include "fileman.h"
#include <stdio.h>
#include <string.h>

void help() {
    printf("Use:\n");
    printf("tfm --create [filename] - to create a file\n");
    printf("tfm --showcontent [filename] - to show a file content\n");
    printf("tfm --remove [filename] - to remove file\n");
    printf("tfm --printdir [dirname] - to display directory contents\n");
    printf("tfm --printdir - to display current directory contents\n");
    printf("tfm --cutpaste [source_file destination_file] - to cut source_file and paste it to destination_file\n");
    printf("tfm --copypaste [source_file destination_file] - to copy source_file and paste it to destination_file\n");
    printf("tfm --linkcreate [old_file new_file] - to create a link\n");
}

int main(int argc, char *argv[]) {
    if (argc == 2) {
        if (strcmp(argv[1], "--printdir") == 0) {
            print_dir(".");
        } else {
            help();
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
        } else {
            help();
        }
    } else if (argc == 4) {
        if (strcmp(argv[1], "--cutpaste") == 0) {
            cut_paste_file(argv[2], argv[3]);
        } else if (strcmp(argv[1], "--copypaste") == 0) {
            copy_paste_file(argv[2], argv[3]);
        } else if (strcmp(argv[1], "--linkcreate") == 0) {
            link_create(argv[2], argv[3]);
        } else {
            help();
        }
    } else {
        help();
    }
    return 0;
}