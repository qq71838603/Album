#ifndef _MYHEAD_H
#define _MYHEAD_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>
#include <dirent.h>
#include <linux/input.h>

typedef struct doublelist
{
	char *pathname;
	struct doublelist *prev;
	struct doublelist *next;
}dl;

int showbmp(char *pathname);
void display(char *pathname, int x, int y, int width, int heigh);
void display1(char *pathname, int x, int y, int width, int heigh);
void display2(char *pathname, int x, int y, int width, int heigh);
void display3(char *pathname, int x, int y, int width, int heigh);
void get_xy(int *lx, int *ly);
void slide(int *fx, int *fy);
void password(void);
int readdirent(char *path);
dl *init_list();
int insert_list(char *path, dl *dllist);
void init_menu(void);
void album(void);
void music(void);

#endif
