#include "myhead.h"

#define BMP_SIZE 800*480*3
#define LCD_SIZE 800*480*4
#define SIZE 800*480

int showbmp(char *pathname)
{
	int i, x, y;
	int bmp_fd = open(pathname, O_RDONLY);
	if(bmp_fd == -1)
	{
		printf("open bmp fail\n");
		exit(-1);
	}

	int lcd_fd = open("/dev/fb0", O_WRONLY);
	if(lcd_fd == -1)
	{
		printf("open lcd fail\n");
		exit(-1);
	}

	char *buf = malloc(BMP_SIZE);
        bzero(buf, BMP_SIZE);

    lseek(bmp_fd, 54, SEEK_SET);
	int ret = read(bmp_fd, buf, BMP_SIZE);
	if(ret == -1)
	{
		printf("read bmp fail\n");
		exit(-1);
	}

	int *bmp_buf = malloc(LCD_SIZE);
	int *lcd_buf = malloc(LCD_SIZE);
	bzero(bmp_buf, LCD_SIZE);
	bzero(lcd_buf, LCD_SIZE);

	for(i=0; i<SIZE; i++)
		bmp_buf[i] = buf[i*3] | buf[i*3+1]<<8 | buf[i*3+2]<<16 | 0x00<<24;

	for(y=0; y<480; y++)
		for(x=0; x<800; x++)
			lcd_buf[y*800+x] = bmp_buf[(479-y)*800+x];

	write(lcd_fd, lcd_buf, LCD_SIZE);

	free(buf);
	free(bmp_buf);
	free(lcd_buf);
	close(bmp_fd);
	close(lcd_fd);
	return 0;
}

