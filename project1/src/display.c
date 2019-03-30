#include "myhead.h"

#define SIZE 800*480*3

void display(char *pathname, int x, int y, int width, int heigh)
{
	int lcd_fd = open("/dev/fb0", O_RDWR);
	if(lcd_fd == -1)
	{
		perror("open lcd fail\n");
		exit(-1);
	}
	
	int bmp_fd = open(pathname, O_RDONLY);
	if(bmp_fd == -1)
	{
		perror("open bmp fail\n");
		exit(-1);
	}

	char *buf = malloc(SIZE);
	memset(buf, 0, SIZE);
	
	lseek(bmp_fd, 54, SEEK_SET);
	int ret = read(bmp_fd, buf, SIZE);
	if(ret == -1)
	{
		perror("read bmp fail\n");
		exit(-1);
	}
	
	//申请内存映射
	int *lcd_ptr = mmap(NULL, 800*480*4, PROT_READ|PROT_WRITE, MAP_SHARED, lcd_fd, 0);
	if(lcd_ptr == MAP_FAILED)
	{
		perror("mmap fail\n");
		exit(-1);
	}
	int *new_ptr = lcd_ptr+800*y+x;//起始坐标(x,y)

	//颠倒，重新分配像素点
	for(y=0; y<heigh; y++)
		for(x=0; x<width; x++)
		{
			new_ptr[((heigh-1-y)*800+x)] = buf[(width*y+x)*3] | buf[(width*y+x)*3+1]<<8 | buf[(width*y+x)*3+2]<<16;
		}
	
	free(buf);
	munmap(lcd_ptr, 800*480*4);//关闭内存映射
	close(bmp_fd);
	close(lcd_fd);
}

void display1(char *pathname, int x, int y, int width, int heigh) //花样1
{
	int lcd_fd = open("/dev/fb0", O_RDWR);
	if(lcd_fd == -1)
	{
		perror("open lcd fail\n");
		exit(-1);
	}
	
	int bmp_fd = open(pathname, O_RDONLY);
	if(bmp_fd == -1)
	{
		perror("open bmp fail\n");
		exit(-1);
	}

	char *buf = malloc(SIZE);
	memset(buf, 0, SIZE);
	
	lseek(bmp_fd, 54, SEEK_SET);
	int ret = read(bmp_fd, buf, SIZE);
	if(ret == -1)
	{
		perror("read bmp fail\n");
		exit(-1);
	}
	
	//申请内存映射
	int *lcd_ptr = mmap(NULL, 800*480*4, PROT_READ|PROT_WRITE, MAP_SHARED, lcd_fd, 0);
	if(lcd_ptr == MAP_FAILED)
	{
		perror("mmap fail\n");
		exit(-1);
	}
	int *new_ptr = lcd_ptr+800*y+x;//起始坐标(x,y)
	
	//颠倒，重新分配像素点
	for(x=0; x<(width/4); x++)
	{
		for(y=0; y<heigh; y++)
		{
			new_ptr[((heigh-1-y)*800+x)] = buf[(width*y+x)*3] | buf[(width*y+x)*3+1]<<8 | buf[(width*y+x)*3+2]<<16;
			new_ptr[((heigh-1-y)*800+x+200)] = buf[(width*y+x+200)*3] | buf[(width*y+x+200)*3+1]<<8 | buf[(width*y+x+200)*3+2]<<16;
			new_ptr[((heigh-1-y)*800+x+400)] = buf[(width*y+x+400)*3] | buf[(width*y+x+400)*3+1]<<8 | buf[(width*y+x+400)*3+2]<<16;
			new_ptr[((heigh-1-y)*800+x+600)] = buf[(width*y+x+600)*3] | buf[(width*y+x+600)*3+1]<<8 | buf[(width*y+x+600)*3+2]<<16;
		}
		usleep(1000);
	}
	
	free(buf);
	munmap(lcd_ptr, 800*480*4);//关闭内存映射
	close(bmp_fd);
	close(lcd_fd);
}

void display2(char *pathname, int x, int y, int width, int heigh) //花样2
{
	int lcd_fd = open("/dev/fb0", O_RDWR);
	if(lcd_fd == -1)
	{
		perror("open lcd fail\n");
		exit(-1);
	}
	
	int bmp_fd = open(pathname, O_RDONLY);
	if(bmp_fd == -1)
	{
		perror("open bmp fail\n");
		exit(-1);
	}

	char *buf = malloc(SIZE);
	memset(buf, 0, SIZE);
	
	lseek(bmp_fd, 54, SEEK_SET);
	int ret = read(bmp_fd, buf, SIZE);
	if(ret == -1)
	{
		perror("read bmp fail\n");
		exit(-1);
	}
	
	//申请内存映射
	int *lcd_ptr = mmap(NULL, 800*480*4, PROT_READ|PROT_WRITE, MAP_SHARED, lcd_fd, 0);
	if(lcd_ptr == MAP_FAILED)
	{
		perror("mmap fail\n");
		exit(-1);
	}
	int *new_ptr = lcd_ptr+800*y+x;//起始坐标(x,y)

	int *bmp_buf = malloc(800*480*4);
	int *lcd_buf = malloc(800*480*4);
	bzero(bmp_buf, 800*480*4);
	bzero(lcd_buf, 800*480*4);

	//颠倒，重新分配像素点
	int i;
	for(i=0; i<800*480; i++)
		bmp_buf[i] = buf[i*3] | buf[i*3+1]<<8 | buf[i*3+2]<<16 | 0x00<<24;
	for(y=0; y<480; y++)
		for(x=0; x<800; x++)
			lcd_buf[y*800+x] = bmp_buf[(479-y)*800+x];
	
	for(y=0; y<(heigh/4); y++)
	{
		for(x=0; x<width; x++)
		{
			new_ptr[y*800+x] = lcd_buf[y*800+x];
			new_ptr[(y+120)*800+x] = lcd_buf[(y+120)*800+x];
			new_ptr[(y+240)*800+x] = lcd_buf[(y+240)*800+x];
			new_ptr[(y+360)*800+x] = lcd_buf[(y+360)*800+x];
		}
		usleep(5000);
	}
	
	free(buf);
	munmap(lcd_ptr, 800*480*4);//关闭内存映射
	close(bmp_fd);
	close(lcd_fd);
}

void display3(char *pathname, int x, int y, int width, int heigh) //花样3
{
	int lcd_fd = open("/dev/fb0", O_RDWR);
	if(lcd_fd == -1)
	{
		perror("open lcd fail\n");
		exit(-1);
	}
	
	int bmp_fd = open(pathname, O_RDONLY);
	if(bmp_fd == -1)
	{
		perror("open bmp fail\n");
		exit(-1);
	}

	char *buf = malloc(SIZE);
	memset(buf, 0, SIZE);
	
	lseek(bmp_fd, 54, SEEK_SET);
	int ret = read(bmp_fd, buf, SIZE);
	if(ret == -1)
	{
		perror("read bmp fail\n");
		exit(-1);
	}
	
	//申请内存映射
	int *lcd_ptr = mmap(NULL, 800*480*4, PROT_READ|PROT_WRITE, MAP_SHARED, lcd_fd, 0);
	if(lcd_ptr == MAP_FAILED)
	{
		perror("mmap fail\n");
		exit(-1);
	}
	int *new_ptr = lcd_ptr+800*y+x;//起始坐标(x,y)

	int *bmp_buf = malloc(800*480*4);
	int *lcd_buf = malloc(800*480*4);
	bzero(bmp_buf, 800*480*4);
	bzero(lcd_buf, 800*480*4);

	//颠倒，重新分配像素点
	int i;
	for(i=0; i<800*480; i++)
		bmp_buf[i] = buf[i*3] | buf[i*3+1]<<8 | buf[i*3+2]<<16 | 0x00<<24;
	for(y=0; y<480; y++)
		for(x=0; x<800; x++)
			lcd_buf[y*800+x] = bmp_buf[(479-y)*800+x];
	
	for(y=0; y<=(heigh/2); y++)
	{
		for(x=0; x<(width/2); x++)
		{
			new_ptr[y*800+x] = lcd_buf[y*800+x];
			new_ptr[(480-y)*800+x] = lcd_buf[(480-y)*800+x];
			new_ptr[(240-y)*800+x+400] = lcd_buf[(240-y)*800+x+400];
			new_ptr[(240+y)*800+x+400] = lcd_buf[(240+y)*800+x+400];
		}
		usleep(1000);
	}

	//free(buf);
	munmap(lcd_ptr, 800*480*4);//关闭内存映射
	close(bmp_fd);
	close(lcd_fd);
}




