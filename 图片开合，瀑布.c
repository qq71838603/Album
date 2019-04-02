#include "myhead.h"

int display1(int *mapBuf,char const *bmppath)        //效果1
{
	int bmpfd;                  //bmp的文件描述符
	char bmpbuf[800*480*3];     //存放读取到的图片数据
	int lcdbuf[800*480];        //用来反转上下颠倒的中间变量
	int tempbuf[800*480];       //存放正确的图片数据
	int ret;
	int i,x,y;
	bzero(bmpbuf,800*480*3);
	bzero(lcdbuf,800*480*4);
	bzero(tempbuf,800*480*4);
    //打开图片
	bmpfd = open(bmppath,O_RDWR);
	if(bmpfd == -1)
	{
		perror("bmp open failed:\n");
		return -1;
	}
	//将bmp文件的54位头信息偏移掉
	lseek(bmpfd,54,SEEK_SET);
	//读取图片的像素点   24位bmp格式的图片在window储存的方式是 一个像素点三位  分别是BGR
	ret = read(bmpfd,bmpbuf,800*480*3);
	close(bmpfd);    //关闭图片文件
	if(ret == -1)
	{
		perror("read gifbmp failed:\n");
		return -1;
	}
	//将BGR转成RGB并且对齐开发板中的一个像素点占四位  即为一个int大小
	for(i=0; i<800*480; i++)
	{
		lcdbuf[i] = bmpbuf[3*i] | bmpbuf[3*i+1]<<8 | bmpbuf[3*i+2]<<16 | 0x00<<24;
	}
	//将图片的上下颠倒翻转过来
	for (x=0; x<800; x++)
	{
		for (y=0; y<480; y++)
		{
			//因为图片是上下颠倒，所以我们可以把lcdbuf中的点对称的放到tempbuf，再将tempbuf写到开发板，这样图片就正常了
			tempbuf[(479-y)*800+x] = lcdbuf[y*800+x];
		}
	}
	//写入到LCD    用write太慢
	//write(lcdfd,tempbuf,800*480*4);
	//把lcd光标复位 
	//lseek(lcdfd,0,SEEK_SET);
	//写到映射的地址
	for (x=0; x<400; x++)
	{
		for (y=0; y<480; y++)
		{
			mapBuf[(399-x)+800*y] = tempbuf[(399-x)+800*y];
			mapBuf[(400+x)+800*y] = tempbuf[(400+x)+800*y];
		}
		usleep(800);
	}
	return 0;
}


int display2(int *mapBuf,char const *bmppath)        //效果2
{
	int bmpfd;                  //bmp的文件描述符
	char bmpbuf[800*480*3];     //存放读取到的图片数据
	int lcdbuf[800*480];        //用来反转上下颠倒的中间变量
	int tempbuf[800*480];       //存放正确的图片数据
	int ret;
	int i,x,y;
	bzero(bmpbuf,800*480*3);
	bzero(lcdbuf,800*480*4);
	bzero(tempbuf,800*480*4);
    //打开图片
	bmpfd = open(bmppath,O_RDWR);
	if(bmpfd == -1)
	{
		perror("bmp open failed:\n");
		return -1;
	}
	//将bmp文件的54位头信息偏移掉
	lseek(bmpfd,54,SEEK_SET);
	//读取图片的像素点   24位bmp格式的图片在window储存的方式是 一个像素点三位  分别是BGR
	ret = read(bmpfd,bmpbuf,800*480*3);
	close(bmpfd);    //关闭图片文件
	if(ret == -1)
	{
		perror("read gifbmp failed:\n");
		return -1;
	}
	//将BGR转成RGB并且对齐开发板中的一个像素点占四位  即为一个int大小
	for(i=0; i<800*480; i++)
	{
		lcdbuf[i] = bmpbuf[3*i] | bmpbuf[3*i+1]<<8 | bmpbuf[3*i+2]<<16 | 0x00<<24;
	}
	//将图片的上下颠倒翻转过来
	for (x=0; x<800; x++)
	{
		for (y=0; y<480; y++)
		{
			//因为图片是上下颠倒，所以我们可以把lcdbuf中的点对称的放到tempbuf，再将tempbuf写到开发板，这样图片就正常了
			tempbuf[(479-y)*800+x] = lcdbuf[y*800+x];
		}
	}
	//写入到LCD    用write太慢
	//write(lcdfd,tempbuf,800*480*4);
	//把lcd光标复位 
	//lseek(lcdfd,0,SEEK_SET);
	//写到映射的地址
	for(y=0; y<480; y++)
	{
		for(x=0; x<800; x++)
		{
			mapBuf[x+800*y] = tempbuf[x];
		}
		for (i=0; i<y; i++)
		{
			for(x=0; x<800; x++)
			{
				mapBuf[x+800*i] = tempbuf[x+800*(i+1)];
			}
		}
	}
	return 0;
}

int display3(int *mapBuf,char const *bmppath)        //效果3
{
	int bmpfd;                  //bmp的文件描述符
	char bmpbuf[800*480*3];     //存放读取到的图片数据
	int lcdbuf[800*480];        //用来反转上下颠倒的中间变量
	int tempbuf[800*480];       //存放正确的图片数据
	int ret;
	int i,x,y;
	bzero(bmpbuf,800*480*3);
	bzero(lcdbuf,800*480*4);
	bzero(tempbuf,800*480*4);
    //打开图片
	bmpfd = open(bmppath,O_RDWR);
	if(bmpfd == -1)
	{
		perror("bmp open failed:\n");
		return -1;
	}
	//将bmp文件的54位头信息偏移掉
	lseek(bmpfd,54,SEEK_SET);
	//读取图片的像素点   24位bmp格式的图片在window储存的方式是 一个像素点三位  分别是BGR
	ret = read(bmpfd,bmpbuf,800*480*3);
	close(bmpfd);    //关闭图片文件
	if(ret == -1)
	{
		perror("read gifbmp failed:\n");
		return -1;
	}
	//将BGR转成RGB并且对齐开发板中的一个像素点占四位  即为一个int大小
	for(i=0; i<800*480; i++)
	{
		lcdbuf[i] = bmpbuf[3*i] | bmpbuf[3*i+1]<<8 | bmpbuf[3*i+2]<<16 | 0x00<<24;
	}
	//将图片的上下颠倒翻转过来
	for (x=0; x<800; x++)
	{
		for (y=0; y<480; y++)
		{
			//因为图片是上下颠倒，所以我们可以把lcdbuf中的点对称的放到tempbuf，再将tempbuf写到开发板，这样图片就正常了
			tempbuf[(479-y)*800+x] = lcdbuf[y*800+x];
		}
	}
	//写入到LCD    用write太慢
	//write(lcdfd,tempbuf,800*480*4);
	//把lcd光标复位 
	//lseek(lcdfd,0,SEEK_SET);
	//写到映射的地址
	for (x=0; x<400; x++)
	{
		for (y=0; y<480; y++)
		{
			mapBuf[x+800*y] = tempbuf[x+800*y];
			mapBuf[(799-x)+800*y] = tempbuf[(799-x)+800*y];
		}
		usleep(800);
	}
	return 0;
}

int display4(int *mapBuf,char const *bmppath)        //效果4
{
	int bmpfd;                  //bmp的文件描述符
	char bmpbuf[800*480*3];     //存放读取到的图片数据
	int lcdbuf[800*480];        //用来反转上下颠倒的中间变量
	int tempbuf[800*480];       //存放正确的图片数据
	int ret;
	int i,x,y;
	bzero(bmpbuf,800*480*3);
	bzero(lcdbuf,800*480*4);
	bzero(tempbuf,800*480*4);
    //打开图片
	bmpfd = open(bmppath,O_RDWR);
	if(bmpfd == -1)
	{
		perror("bmp open failed:\n");
		return -1;
	}
	//将bmp文件的54位头信息偏移掉
	lseek(bmpfd,54,SEEK_SET);
	//读取图片的像素点   24位bmp格式的图片在window储存的方式是 一个像素点三位  分别是BGR
	ret = read(bmpfd,bmpbuf,800*480*3);
	close(bmpfd);    //关闭图片文件
	if(ret == -1)
	{
		perror("read gifbmp failed:\n");
		return -1;
	}
	//将BGR转成RGB并且对齐开发板中的一个像素点占四位  即为一个int大小
	for(i=0; i<800*480; i++)
	{
		lcdbuf[i] = bmpbuf[3*i] | bmpbuf[3*i+1]<<8 | bmpbuf[3*i+2]<<16 | 0x00<<24;
	}
	//将图片的上下颠倒翻转过来
	for (x=0; x<800; x++)
	{
		for (y=0; y<480; y++)
		{
			//因为图片是上下颠倒，所以我们可以把lcdbuf中的点对称的放到tempbuf，再将tempbuf写到开发板，这样图片就正常了
			tempbuf[(479-y)*800+x] = lcdbuf[y*800+x];
		}
	}
	//写入到LCD    用write太慢
	//write(lcdfd,tempbuf,800*480*4);
	//把lcd光标复位 
	//lseek(lcdfd,0,SEEK_SET);
	//写到映射的地址
	for (x=0; x<800; x++)
	{
		for (y=0; y<480; y++)
		{
			mapBuf[x+y*800] = tempbuf[x+y*800];
		}
		usleep(1*1000);
	}

	return 0;
}