#include "head.h"

int type(char *name,int change)
{
	int picfd=open(name,O_RDONLY);
	if(picfd==-1)
	{
		printf("打开图片%s\n",name);
		return -1;
	}
	
	int lcdfd=open("/dev/fb0",O_RDWR);
	if(lcdfd==-1)
	{
		printf("打开显示屏！\n");
		return -1;
	}
	
	long width,height;
	lseek(picfd,18,SEEK_SET);
	read(picfd,&width,sizeof(long));
	lseek(picfd,22,SEEK_SET);
	read(picfd,&height,sizeof(long));

	int *wrbuf=NULL;
	
	char rdbuf[width*height*3];
	bzero(rdbuf,width*height*3);
	lseek(picfd,54,SEEK_SET);
	read(picfd,rdbuf,width*height*3);
	
	wrbuf=mmap(0,800*480*4,PROT_WRITE|PROT_READ,MAP_SHARED,lcdfd,0);
	
	int i,j;
	int r;
	int i1,j1;
	int n,s;
	int remainder,quotient;
	
	switch(change)
	{
		case 1:	                   //圆形显示
				for(r=1;r<480;r++)
				for(i=0;i<height;i++)
				for(j=0;j<width;j++)
				{	
					if(((i-240)*(i-240)+(j-400)*(j-400))<=r*r)
					wrbuf[(height-i-1)*800+j]=rdbuf[(i*width+j)*3]|rdbuf[(i*width+j)*3+1]<<8|rdbuf[(i*width+j)*3+2]<<16|0x00<<24;
				}
				sleep(1);
				break;
	    case 2:	for(j=0;j<width/8;j++)   //8栅格显示
				{
					for(i=0;i<height;i++)
					{	
						wrbuf[(height-i-1)*800+j]=rdbuf[(i*width+j)*3]|rdbuf[(i*width+j)*3+1]<<8|rdbuf[(i*width+j)*3+2]<<16|0x00<<24;
						wrbuf[(height-i-1)*800+j+100]=rdbuf[(i*width+j+100)*3]|rdbuf[(i*width+j+100)*3+1]<<8|rdbuf[(i*width+j+100)*3+2]<<16|0x00<<24;
						wrbuf[(height-i-1)*800+j+200]=rdbuf[(i*width+j+200)*3]|rdbuf[(i*width+j+200)*3+1]<<8|rdbuf[(i*width+j+200)*3+2]<<16|0x00<<24;
						wrbuf[(height-i-1)*800+j+300]=rdbuf[(i*width+j+300)*3]|rdbuf[(i*width+j+300)*3+1]<<8|rdbuf[(i*width+j+300)*3+2]<<16|0x00<<24;
						wrbuf[(height-i-1)*800+j+400]=rdbuf[(i*width+j+400)*3]|rdbuf[(i*width+j+400)*3+1]<<8|rdbuf[(i*width+j+400)*3+2]<<16|0x00<<24;
						wrbuf[(height-i-1)*800+j+500]=rdbuf[(i*width+j+500)*3]|rdbuf[(i*width+j+500)*3+1]<<8|rdbuf[(i*width+j+500)*3+2]<<16|0x00<<24;
						wrbuf[(height-i-1)*800+j+600]=rdbuf[(i*width+j+600)*3]|rdbuf[(i*width+j+600)*3+1]<<8|rdbuf[(i*width+j+600)*3+2]<<16|0x00<<24;
						wrbuf[(height-i-1)*800+j+700]=rdbuf[(i*width+j+700)*3]|rdbuf[(i*width+j+700)*3+1]<<8|rdbuf[(i*width+j+700)*3+2]<<16|0x00<<24;
					}
					usleep(8000);
				}
				sleep(1);
				break;
		case 3:	for(i=height-1;i>=height/2;i--)//右上
				{
					for(j=width/2;j<width;j++)
					{	
						wrbuf[(height-i-1)*800+j]=rdbuf[(i*width+j)*3]|rdbuf[(i*width+j)*3+1]<<8|rdbuf[(i*width+j)*3+2]<<16|0x00<<24;
					}
					usleep(10000);
				}
				for(i=0;i<height/2;i++)//左下
				{
					for(j=0;j<width/2;j++)
					{	
						wrbuf[(height-i-1)*800+j]=rdbuf[(i*width+j)*3]|rdbuf[(i*width+j)*3+1]<<8|rdbuf[(i*width+j)*3+2]<<16|0x00<<24;
					}
					usleep(10000);
				}
				for(j=width-1;j>=width/2;j--)
				{
					for(i=0;i<height/2;i++)//右下
					{	
						wrbuf[(height-i-1)*800+j]=rdbuf[(i*width+j)*3]|rdbuf[(i*width+j)*3+1]<<8|rdbuf[(i*width+j)*3+2]<<16|0x00<<24;
					}
					usleep(10000);
				}
				for(j=0;j<width/2;j++)//左上
				{
					for(i=height/2;i<height;i++)
					{	
						wrbuf[(height-i-1)*800+j]=rdbuf[(i*width+j)*3]|rdbuf[(i*width+j)*3+1]<<8|rdbuf[(i*width+j)*3+2]<<16|0x00<<24;
					}
					usleep(10000);
				}
				sleep(1);
				break;
		case 4:	         //瀑布
				for(i=0;i<height;i++)
				{
					for(j=0;j<width;j++)
					{	
						wrbuf[j]=rdbuf[(i*width+j)*3]|rdbuf[(i*width+j)*3+1]<<8|rdbuf[(i*width+j)*3+2]<<16|0x00<<24;
					}
					
					if(i==height-1)
						break;
					for(i1=i+1;i1>0;i1--)
					{
						for(j1=0;j1<width;j1++)
						{
							wrbuf[i1*800+j1]=wrbuf[(i1-1)*800+j1];
						}
					}
				}
				sleep(1);
				break;
		case 5:       //回形环绕
				
				s=30;
				quotient=480/(2*s);
				remainder=480%(s*2);
				for(n=0;n<quotient;n++)
				{
					for(j=width-1-s*n;j>=s*n;j--)
					{
						for(i=s*n;i<s*(n+1);i++)
						{	
							wrbuf[i*800+j]=rdbuf[((height-i-1)*width+j)*3]|rdbuf[((height-i-1)*width+j)*3+1]<<8|rdbuf[((height-i-1)*width+j)*3+2]<<16|0x00<<24;
						}
						usleep(10);
					}
					for(i=s*(n+1);i<height-s*n;i++)
					{
						for(j=s*n;j<s*(n+1);j++)
						{	
							wrbuf[i*800+j]=rdbuf[((height-i-1)*width+j)*3]|rdbuf[((height-i-1)*width+j)*3+1]<<8|rdbuf[((height-i-1)*width+j)*3+2]<<16|0x00<<24;
						}
						usleep(10);
					}
					for(j=s*(n+1);j<width-s*n;j++)
					{
						for(i=height-s*(n+1);i<height-s*n;i++)
						{	
							wrbuf[i*800+j]=rdbuf[((height-i-1)*width+j)*3]|rdbuf[((height-i-1)*width+j)*3+1]<<8|rdbuf[((height-i-1)*width+j)*3+2]<<16|0x00<<24;
						}
						usleep(10);
					}
					for(i=height-1-s*(n+1);i>=s*(n+1);i--)
					{
						for(j=width-s*(n+1);j<width-s*n;j++)
						{	
							wrbuf[i*800+j]=rdbuf[((height-i-1)*width+j)*3]|rdbuf[((height-i-1)*width+j)*3+1]<<8|rdbuf[((height-i-1)*width+j)*3+2]<<16|0x00<<24;
						}
						usleep(10);
					}
				}
				if(remainder>s)
				{
					for(j=width-1-s*n;j>=s*n;j--)
					{
						for(i=s*n;i<s*(n+1);i++)
						{	
							wrbuf[i*800+j]=rdbuf[((height-i-1)*width+j)*3]|rdbuf[((height-i-1)*width+j)*3+1]<<8|rdbuf[((height-i-1)*width+j)*3+2]<<16|0x00<<24;
						}
						usleep(10);
					}
					for(i=s*(n+1);i<height-s*n;i++)
					{
						for(j=s*n;j<s*(n+1);j++)
						{	
							wrbuf[i*800+j]=rdbuf[((height-i-1)*width+j)*3]|rdbuf[((height-i-1)*width+j)*3+1]<<8|rdbuf[((height-i-1)*width+j)*3+2]<<16|0x00<<24;
						}
						usleep(10);
					}
					for(j=s*(n+1);j<width-s*n;j++)
					{
						for(i=s*(n+1);i<height-s*n;i++)
						{	
							wrbuf[i*800+j]=rdbuf[((height-i-1)*width+j)*3]|rdbuf[((height-i-1)*width+j)*3+1]<<8|rdbuf[((height-i-1)*width+j)*3+2]<<16|0x00<<24;
						}
						usleep(10);
					}
				}
				else if(remainder<=s&&remainder>0)
				{
					for(j=width-1-s*n;j>=s*n;j--)
					{
						for(i=s*n;i<height-s*n;i++)
						{	
							wrbuf[i*800+j]=rdbuf[((height-i-1)*width+j)*3]|rdbuf[((height-i-1)*width+j)*3+1]<<8|rdbuf[((height-i-1)*width+j)*3+2]<<16|0x00<<24;
						}
						usleep(10);
					}
				}
				sleep(1);
				break;
	    case 6:     //上半圆左右显示
				for(r=0;r<640;r++)
				{
					for(i=0;i<height;i++)
						for(j=0;j<width;j++)
						{	
							if(((i-479)*(i-479)+j*j<=r*r)||((i-479)*(i-479)+(j-799)*(j-799)<=r*r))
							wrbuf[(height-i-1)*800+j]=rdbuf[(i*width+j)*3]|rdbuf[(i*width+j)*3+1]<<8|rdbuf[(i*width+j)*3+2]<<16|0x00<<24;
						}
				}
				sleep(1);
				break;
	}		
    int i1,j1;         //俄罗斯方块
		for(i=0;i<height;i++)  //田方块
		{
			if(i<120)
			{
				for(j=0;j<200;j++)
				{	
					wrbuf[j]=rdbuf[(i*width+j)*3]|rdbuf[(i*width+j)*3+1]<<8|rdbuf[(i*width+j)*3+2]<<16|0x00<<24;
				}
			}
			if(i==height-1)
				break;
			for(i1=i+1;i1>0;i1--)
			{
				if(i1>i-120)
				for(j1=0;j1<200;j1++)
				{
					wrbuf[i1*800+j1]=wrbuf[(i1-1)*800+j1];
				}
				else 
				for(j1=0;j1<200;j1++)
				{
					wrbuf[i1*800+j1]=0xffffff;
				}	
			}
		}
	    
		for(i=0;i<height;i++)  //反L方块
		{
			if(i<60)
			{
				for(j=200;j<400;j++)
				{	
					wrbuf[j]=rdbuf[(i*width+j)*3]|rdbuf[(i*width+j)*3+1]<<8|rdbuf[(i*width+j)*3+2]<<16|0x00<<24;
				}
			}
			if(i>=60&&i<240)
			{
				for(j=300;j<400;j++)
				{	
					wrbuf[j]=rdbuf[(i*width+j)*3]|rdbuf[(i*width+j)*3+1]<<8|rdbuf[(i*width+j)*3+2]<<16|0x00<<24;
				}
			}
			if(i==height-1)
				break;
			for(i1=i+1;i1>0;i1--)
			{
				if(i1>i-60)
				{
					for(j1=200;j1<400;j1++)
					{
						wrbuf[i1*800+j1]=wrbuf[(i1-1)*800+j1];
					}
			    }
				else if(i1<=i-60&&i1>i-240)
				{
					for(j1=200;j1<400;j1++)
					{
						if(j1>=200&&j1<300)
						wrbuf[i1*800+j1]=0xffffff;
                        else
						wrbuf[i1*800+j1]=wrbuf[(i1-1)*800+j1];
					}
				}
				else
				{
					for(j1=200;j1<400;j1++)
					{
						wrbuf[i1*800+j1]=0xffffff;
					}
				}				
			}
		}
	
	    for(i=0;i<height;i++)  //横长方块
		{
			if(i<60)
			{
				for(j=400;j<800;j++)
				{	
					wrbuf[j]=rdbuf[(i*width+j)*3]|rdbuf[(i*width+j)*3+1]<<8|rdbuf[(i*width+j)*3+2]<<16|0x00<<24;
				}
			}
			if(i==height-1)
				break;
			for(i1=i+1;i1>0;i1--)
			{
				if(i1>i-60)
				for(j1=400;j1<800;j1++)
				{
					wrbuf[i1*800+j1]=wrbuf[(i1-1)*800+j1];
				}
				else 
				for(j1=400;j1<800;j1++)
				{
					wrbuf[i1*800+j1]=0xffffff;
				}	
			}
		}
	    
	    for(i=0;i<height-120;i++) //竖长方块
		{
			if(i<240)
			{
				for(j=0;j<100;j++)
				{	
					wrbuf[j]=rdbuf[(i*width+j)*3]|rdbuf[(i*width+j)*3+1]<<8|rdbuf[(i*width+j)*3+2]<<16|0x00<<24;
				}
			}
			if(i==height-1-120)
				break;
			for(i1=i+1;i1>0;i1--)
			{
				if(i1>i-240)
				for(j1=0;j1<100;j1++)
				{
					wrbuf[i1*800+j1]=wrbuf[(i1-1)*800+j1];
				}
				else 
				for(j1=0;j1<100;j1++)
				{
					wrbuf[i1*800+j1]=0xffffff;
				}	
			}
		}
	    
	    for(i=0;i<height-60;i++) //ㄣ方块
		{
			if(i<60)
			{
				for(j=200;j<300;j++)
				{	
					wrbuf[j]=rdbuf[(i*width+j)*3]|rdbuf[(i*width+j)*3+1]<<8|rdbuf[(i*width+j)*3+2]<<16|0x00<<24;
				}
			}
			else if(i>=60&&i<120)
			{
				for(j=100;j<300;j++)
				{	
					wrbuf[j]=rdbuf[(i*width+j)*3]|rdbuf[(i*width+j)*3+1]<<8|rdbuf[(i*width+j)*3+2]<<16|0x00<<24;
				}
			}
			else if(i>=120&&i<180)
			{
				for(j=100;j<200;j++)
				{	
					wrbuf[j]=rdbuf[(i*width+j)*3]|rdbuf[(i*width+j)*3+1]<<8|rdbuf[(i*width+j)*3+2]<<16|0x00<<24;
				}
			}
			if(i==height-1-60)
				break;
			for(i1=i+1;i1>0;i1--)
			{
				if(i1>i-60)
				for(j1=200;j1<300;j1++)
				{
					wrbuf[i1*800+j1]=wrbuf[(i1-1)*800+j1];
				}
				else if(i1<=i-60&&i1>i-120)
				{
					wrbuf[i1*800+j1]=wrbuf[(i1-1)*800+j1];
				}
				else if(i1<=i-120&&i1>i-180)
				{
					wrbuf[i1*800+j1]=wrbuf[(i1-1)*800+j1];
				}
				else
				for(j1=100;j1<300;j1++)
				{
					wrbuf[i1*800+j1]=0xffffff;
				}	
			}
		}
	    
	
	
	
	
	close(picfd);
	close(lcdfd);
	return 0;
}