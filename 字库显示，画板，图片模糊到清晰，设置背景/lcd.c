
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "project.h"  //  #include <xxx.h> 默认搜索标准库的头文件路径  /usr/include
					  //  #include "xxx.h" 默认先搜索当前路径，再搜索标准库的头文件路径  /usr/include

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "font_libs.h"

int LCD_fd;
unsigned long *fb_mem  = NULL;
unsigned char *bmp_buf = NULL;


 int Init_Lcd(void)
 {
		
		int ret;
		
		//申请LCD操作许可、
		LCD_fd = open("/dev/fb0",O_RDWR);
		if(LCD_fd == -1)
		{
			perror("Open LCD");
			return -1;
		}
	 
		
		//显存映射
       fb_mem = (unsigned long *)mmap(NULL,    //程序空间的映射地址 ， 不确定可以赋值NULL，由系统分配
									  FB_SIZE, //映射的内存的大小    LCD 800*480*4
									  PROT_READ |  PROT_WRITE,         //映射区的保护操作           PROT_EXEC  Pages may be executed.
																	   //                           PROT_READ  Pages may be read.
																	   //                           PROT_WRITE Pages may be written.
																	   //							PROT_NONE  Pages may not be accessed.
									  MAP_SHARED,       //共享标志            MAP_SHARED     MAP_PRIVATE
									  LCD_fd,           //目标文件描述符 ----> 内存映射的目标文件
									  0);   //映射内存的偏移大小   默认从头开始，0
									  
		if(fb_mem == MAP_FAILED)	
		{
			perror("Mmap LCD");
			return -1;	
		}
		
		//Show_Bmp(0,0,"logo.bmp");
		
		return 0;
	 
 }
 
 
 void  UnInit_Lcd(void)
 {
	
	 free(bmp_buf);
	 //撤销映射
     munmap(fb_mem,   //映射后的地址，通过mmap返回的值
		   FB_SIZE); //映射的大小	
		   
	 close(LCD_fd); 
 }
 
 

 
 int Show_Bmp(int X, int Y,const char *bmpfile)
 {
		FILE *filp;
		int x,y,i,ret;
		int width,height,bits;
		BMPFH  BmpFH;  	 //BMP文件信息头
		BMPIH  BmpIH;    //BMP位图信息头
		
		//加载一张BMP位图
		// 打开BMP，获得操作许可
		filp = fopen(bmpfile,"r");
		if(filp == NULL)
		{
			perror("Fppen BMP");
			return -1;
		}	

		
		//获取BMP的文件信息头  14BYTE
		fread(&BmpFH,sizeof(BMPFH),1,filp); 
		//printf("TYPE: %c",BmpFH.bfType & 0xFF);     //显示bfType的低字节
		//printf("%c \n",(BmpFH.bfType >> 8) & 0xFF); //显示bfType的高字节
		//printf("FILE SIZE: %d \n",BmpFH.bfSize);
		//获取BMP的位图信息头
		fread(&BmpIH,sizeof(BMPIH),1,filp);
		//printf("BIT: %d \n",BmpIH.biBitCount);
		//printf("RGB SIZE: %d \n",BmpIH.biSizeImage);	
		//printf("x width: %d \n",BmpIH.biWidth);
		//printf("x heigth: %d \n",BmpIH.biHeight);
		
		width   = BmpIH.biWidth;
		height  = BmpIH.biHeight;
		bits    = BmpIH.biBitCount>>3;  // 相当于 BmpIH.biBitCount/8 ，获得一个像素对应的字节数
		
		bmp_buf = (unsigned char *)malloc(BmpIH.biSizeImage);
		memset(bmp_buf,0x0,BmpIH.biSizeImage);	
		
		
  		//获取BMP位图的RGB信息
		fread(bmp_buf,BmpIH.biSizeImage,1,filp);
		
		//如果该图片的每一行的大小不是4字节的倍数，则图片将有额外字节填充，须对齐
		int index;
		int linebyte = (BmpIH.biSizeImage/height); //获取数据对齐的标准下的每行像素数据字节数
			
		
		//将24bit的RGB填充到32bit的ARGB中
		for(index=0,y=(479-Y-height),i=0;y<(479-Y);y++,index+=linebyte)
		{
			for(i=index,x=X;x<X+width;x++,i+=3)
			{				
				fb_mem[(479-y)*800+x]  =  (bmp_buf[i] |  bmp_buf[i+1]<<8  |  bmp_buf[i+2] <<16);
			}

		}
		 // *(fb_mem+(479-y)*800+x)   fb_mem 作为显存的首地址，通过指针偏移操作，访问对应的像素位置
	      
		//写LCD
		//尺寸 7 寸   分辨率: 800*480 色深（位）： ARGB  16bit（565; 5:5:5:1）   24bit(8:8:8)  32bit(8:8:8:8)
		// 黑色： 0x00000000   白色： 0xFFFFFFFF  红色：0xFF0000
	 
		fclose(filp);
	 
	 return 0;
 }
 
 
 int Show_Msg(int x,int y,char *msg)
 {
	int i=0;
	char str[50];
	char cn;
	
	while( (cn = *(msg++))!= '\0')
	{
		
		if((cn > 47 && cn < 58) || (cn >96 && cn<123))
		{
			memset(str,0,sizeof(str));
			sprintf(str,"keyboard/%c.bmp",cn);
			Show_Bmp(x+i++*50, y,str);
		}

	}	 
	
	sleep(2);
	return 0;
	 	 
 }
 
 
int  Create_LineEdit(int X,int Y,int width,int height)
{
	int x,y;
	
	for(y=Y;y<Y+height;y++)
	{
		for(x=X;x<X+width;x++)
		{				
			fb_mem[y*800+x]  =  0xFFFFFFFF;
		}

	}	
	
	return 0;		
}


int  Clean_Area(int X,int Y,int width,int height)
{
	int x,y;
	
	for(y=Y;y<Y+height;y++)
	{
		for(x=X;x<X+width;x++)
		{				
			fb_mem[y*800+x]  =  0xFFFFFFFF;
		}

	}	
	return 0;		
}




int  Draw_Text16(unsigned int x,unsigned int y,unsigned long color,const unsigned char ch[])
{
	unsigned short int i,j;
	unsigned char mask,buffer;
	for(i=0;i<16;i++)
	{
		mask =0x80;				//掩码
		buffer =ch[i*2];		//提取一行的第一个字节
		for(j=0;j<8;j++)
		{
			if(buffer &mask)
			{
				fb_mem[(y+i)*800+(x+j)]= color;	//为画笔上色	
			}
			mask =mask >>1;
		}
		mask =0x80;
		buffer =ch[i*2+1];
		for(j=0;j<8;j++)
		{
			if(buffer &mask)
			{
				fb_mem[(y+i)*800+(x+j+8)]= color;
			}
			mask =mask>>1;	
		}		
	}
	
	return 0;
}


int  Draw_TextX(unsigned int x,unsigned int y,unsigned long color,const unsigned char ch[],int size)
{
	unsigned short int i,j,k,m;
	unsigned char mask,buffer;
	for(i=0;i<16;i++)
	{
		mask =0x80;				//掩码
		buffer =ch[i*2];		//提取一行的第一个字节
		for(j=0;j<8;j++)
		{
			if(buffer &mask)
			{
			  for(k=0;k<size;k++)
			  	{
			  		for(m=0;m<size;m++)
			  		{
					  fb_mem[(y+i*size+m)*800+(x+j*size+k)]= color;
			  		}
			  	}
			}
			mask =mask >>1;
		}
		mask =0x80;
		buffer =ch[i*2+1];
		for(j=0;j<8;j++)
		{
			if(buffer &mask)
			{
			  for(k=0;k<size;k++)
			  	{	
			  		for(m=0;m<size;m++)
			  		{	
					   fb_mem[(y+i*size+m)*800+(x+(j+8)*size+k)]= color;
			  		}
			  	}
			}
			mask =mask>>1;	
		}		
	}
	
	return 0;
}


int  Draw_ASCII(unsigned int x,unsigned int y,unsigned long color,const unsigned char ch[])
{
       unsigned short int i,j;
       unsigned char mask,buffer;
      
       for(i=0;i<16;i++)
       {
              mask=0x80;
              buffer=ch[i];
              for(j=0;j<8;j++)
              {                   
                     if(buffer&mask)
                     {
                            fb_mem[(y+i)*800+(x+j)]= color;
                     }
                     mask=mask>>1;                   
              }
       }
	   
	   return 0;
}


int  Draw_ASCIIX(unsigned int x,unsigned int y,unsigned long color,const unsigned char ch[],int size)
{
       unsigned short int i,j,k,m;
       unsigned char mask,buffer;
      
       for(i=0;i<16;i++)
       {
              mask=0x80;
              buffer=ch[i];
              for(j=0;j<8;j++)
              {                   
                     if(buffer&mask)
                     {
						 for(k=0;k<size;k++)
						   {
							   for(m=0;m<size;m++)
							   {
							      fb_mem[(y+i*size+m)*800+(x+j*size+k)]= color;
							   }
						   }

                     }
                     mask=mask>>1;                   
              }
       }
	   
	   return 0;
}


int  Display_character(unsigned int x,unsigned int y,unsigned int len,unsigned char *string)
{
	int k, xx;
	unsigned char qh,wh;
	const unsigned char *mould;
	unsigned int length =len;
	
	for(k=0,xx=x;k<length-1;k++)
	{
		if(string[k]&0x80)   //中文字符
		{
			qh =string[k]-0xa0;			//区号
			wh =string[k+1]-0xa0;		//位号
			mould =&__MYCHS[((qh-1)*94+wh-1)*32];
			Draw_Text16(4+xx,y,0xFF0000,mould);
			xx+=16;
			k++;
		}
		else
		{
			mould =&__ASCII[string[k]*16];
			Draw_ASCII(4+xx,y,0xFF0000,mould);
			xx+=8;
		}
	}
	return 0;
}

int  Display_characterX(unsigned int x,unsigned int y,unsigned int len,unsigned char *string,int size)
{
	int k, xx;
	unsigned char qh,wh;
	const unsigned char *mould;
	unsigned int length =len;
	
	for(k=0,xx=x;k<length-1;k++)
	{
		if(string[k]&0x80)   //中文字符
		{
			qh =string[k]-0xa0;			//区号
			wh =string[k+1]-0xa0;		//位号
			mould =&__MYCHS[((qh-1)*94+wh-1)*32];
			Draw_TextX(4*size+xx,y,0xff0000,mould,size); //加4为了让每个中文之间有一定的间隙
			xx+=16*size;//当前的中文字模为32*32,每次显示下一个中文时横向偏移32个bit
			k++; //加载下一个中文，两次++操作，偏移2个字节
		}
		else
		{
			mould =&__ASCII[string[k]*16];
			Draw_ASCIIX(4*size+xx,y,0xff0000,mould,size);
			xx+=8*size;//当前的ASCII字模显示为8*16,每次显示下一个中文时横向偏移8个bit
		}
	}
	return 0;
}

int  Display_characterXX(unsigned int x,unsigned int y,unsigned int len,unsigned char *string,int size,unsigned long color)
{
	int k, xx;
	unsigned char qh,wh;
	const unsigned char *mould;
	unsigned int length =len;
	
	for(k=0,xx=x;k<length-1;k++)
	{
		if(string[k]&0x80)   //中文字符
		{
			qh =string[k]-0xa0;			//区号
			wh =string[k+1]-0xa0;		//位号
			mould =&__MYCHS[((qh-1)*94+wh-1)*32];
			Draw_TextX(4*size+xx,y,color,mould,size); //加4为了让每个中文之间有一定的间隙
			xx+=16*size;//当前的中文字模为32*32,每次显示下一个中文时横向偏移32个bit
			k++; //加载下一个中文，两次++操作，偏移2个字节
		}
		else
		{
			mould =&__ASCII[string[k]*16];
			Draw_ASCIIX(4*size+xx,y,color,mould,size);
			xx+=8*size;//当前的ASCII字模显示为8*16,每次显示下一个中文时横向偏移8个bit
		}
	}
	return 0;
}


int Roll_Dispaly(unsigned char *str)
{
	int x;
	
	for(x=600;x>20;x--)
	{
	  Display_characterXX(x,45,strlen(str)+1,str,4,0xFF0000);
	  usleep(50000);
	  Display_characterXX(x+1,45,strlen(str)+1,str,4,0xFFFFFFFF); 
    }
	
	Display_characterXX(x+1,45,strlen(str)+1,str,4,0xFFFFFFFF);
	return 0;
}

