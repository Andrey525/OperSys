#pragma once

int create_file(const char *pathToFile); // директория должна существовать
int view_file_content(const char *pathToFile);
int remove_file(const char *pathToFile);
int cut_paste_file(const char *srcPathToFile, const char *dstPathToFile);
int copy_paste_file(const char *srcPathToFile, const char *dstPathToFile);
int link_create(const char *oldPathToFile, const char *newPathToFile);
int print_dir(const char *path);