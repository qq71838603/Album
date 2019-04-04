#include"commonheader.h"
#include"linkedlist.h"
#include <setjmp.h>
#include"lcd.c"
/* #include"desktop_list.h" */
stu *mp3head;
stu *bmphead;
int dirsol(char *argv);
//所有的全局变量
/* char mp3arr[50][50],bmparr[200][30];
int mp3flag,txtflag,bmpflag; */
int ts_fd,lcd_fd,*fbp;
struct input_event buf;
short system_stat;
short xpos,ypos,press;
int *scrbuffer;
short exitevent=0;
short ifshowacrossbar=0;
char background_image[255]="/mydesk/source/mainpic.bmp";

void *across();

jmp_buf _main,_sub1;

pthread_t first_pthread;
pthread_t bottompth;
pthread_t exitbutton;
pthread_t acrossbar;
pthread_t ifright;
/* desktop *deskhead; */
//屏幕初始化
int screen_init()
{
	lcd_fd = open("/dev/fb0",O_RDWR);	
	if(lcd_fd == -1)
	{
		//_printf("open touch_screen fail\n");
		return -1;
	}
	ts_fd = open("/dev/input/event0",O_RDWR);
	if(ts_fd == -1)
	{
		//_printf("open touch_screen fail\n");
		return -1;
	}
	//2 映射显存，对显存进行操作就相当于对屏幕进行操作
	fbp = (int *)mmap(0,800*480*4,PROT_WRITE | PROT_READ,MAP_SHARED,lcd_fd,0);
	scrbuffer=malloc(480*800*4);
	system_stat=1;
}
int clearscr(int x1,int y1,int x2,int y2)
{
	int i,j;
	for(i=y1;i<y2;i++)
	{
		for(j=x1;j<x2;j++)
		{
		*(fbp+i*800+j)=0x00000000;
		}
	}
	return 0;
}
int writescr(int x1,int y1,int x2,int y2,int color)
{	
	int i,j;
	for(i=y1;i<y2;i++)
	{
		for(j=x1;j<x2;j++)
		{
			if(j>=0&&j<800 && i>=0&&i<480)
			*(fbp+i*800+j)=color;
		}
	}
}
//打开目录,并且读取目录内容
int dirsol(char *argv)
{
	struct dirent *dirinfo;
	//_printf("dir is %s\n",argv);
	DIR *dp=opendir(argv);
	if(dp==NULL)
	{
		//_printf("Error in directory opening!\n");
		return -1;
	}
	char *tmp=malloc(100);
	bzero(tmp,100);
	strcpy(tmp,argv);
	char buf[255];
	//读取目录里面的内容
	while((dirinfo = readdir(dp))!=NULL)
	{
		if(strncmp(dirinfo->d_name,".",1)==0)
			continue;
		
		//_printf("%s\n",dirinfo->d_name);
		if(dirinfo->d_type==DT_DIR)
		{
			////_printf("a dir %s\n",dirinfo->d_name);
			sprintf(tmp,"%s%s/",argv,dirinfo->d_name);
			dirsol(tmp);
		}
 		else if(strstr(dirinfo->d_name,".mp3")!=NULL)
		{
			bzero(buf,255);
			sprintf(buf,"%s%s",tmp,dirinfo->d_name);
			tail_insert(buf,mp3head);
			////_printf("mp3!\n",buf);
		}
		
		else if(strstr(dirinfo->d_name,".bmp")!=NULL)
		{
			bzero(buf,255);
			sprintf(buf,"%s%s",tmp,dirinfo->d_name);
			tail_insert(buf,bmphead);
			////_printf("bmpfile :%s\n",buf);
		}
		
	}
	system_stat=1;
	closedir(dp);
	free(tmp);
	return 0;
}



int *show_bmp(char * bmppathname,int x1,int y1,int z)
{
	int i,x,y,k;
	//定义一个用来存放图片头文件信息的数组
	char bmp_head[54] = {0};
	//定义两个变量，用来保存从头信息中获取到的图片的宽和高
	unsigned short w=0,h=0;
	//------------1-----------------
	//打开BMP图片
	int bmp_fd = open(bmppathname,O_RDWR);
	if(bmp_fd == -1)
	{
		//_printf("open bmp fail\n");
		//_printf("%s\n",strerror(errno));
		return (int *)-1;
	}	
	//2读取BMP的数据
	//读取头信息，获取图片的宽和高
	read(bmp_fd,bmp_head,54);
	w = bmp_head[19] << 8 | bmp_head[18];
	h = bmp_head[23] << 8 | bmp_head[22];
	////_printf("w:%d h:%d\n",w,h);
	//判断要刷的图片是否超过了屏幕范围
	if(w+x1>800 || h+y1>480)
	{
		//_printf("yuejie\n");
		return (int *)-1;
	}
	//根据得到的宽和高来设计合理的数组
	char bmp_buf[w*h*3];
	int touch_screen_buf[w*h];
	//读取BMP的像素数据
	int ret = read(bmp_fd,bmp_buf,w*h*3);
	if(ret == -1)
	{
		//_printf("read bmp fail\n");
		return (int *)-1;
	}	
	//-------------3--------------
	//对齐像素
	for(i=0;i<w*h;i++)
		touch_screen_buf[i] = bmp_buf[3*i+0] | bmp_buf[3*i+1]<<8 | bmp_buf[3*i+2]<<16 | 0x00<<24; 
	//在显存中写入数据，相当于显示在touch_screen屏幕上
	switch(z)
	{
		case 0:
		for(y=0;y<h;y++)
		{
			for(x=0;x<w;x++)
			{		
				*(fbp+800*(y+y1)+x+x1)= touch_screen_buf[(h-1-y)*w+x];//把图片像素填充到显存中去，让它在touch_screen屏幕上显示出来
			}
		}
		break;
		case 1:
		//从上到下的显示效果
		for(y=0;y<h;y++)
		{
			for(x=0;x<w;x++)
			{		
				*(fbp+800*(y+y1)+x+x1)= touch_screen_buf[(h-1-y)*w+x];//把图片像素填充到显存中去，让它在touch_screen屏幕上显示出来
			}
			usleep(1000);//停顿5毫秒，让我们能够观察到图片是如何在touch_screen上面显示出来的
		}
		break;
		case 2:
		//从左往右的显示效果
		for(x=0;x<w;x++)
		{
			for(y=0;y<h;y++)
			{		
				*(fbp+800*(y+y1)+x+x1)= touch_screen_buf[(h-1-y)*w+x];//把图片像素填充到显存中去，让它在touch_screen屏幕上显示出来

			}
			usleep(1000);//停顿5毫秒，让我们能够观察到图片是如何在touch_screen上面显示出来的
		}
		break;
		case 3:
		//从下到上：
		for(y=0;y<h;y++)
		{
			for(x=0;x<w;x++)
			{		
				*(fbp + (h-y-1+y1)*800 + x1 + x) = touch_screen_buf[y*w+x];//把图片像素填充到显存中去，让它在touch_screen屏幕上显示出来

			}
			usleep(1000);//停顿5毫秒，让我们能够观察到图片是如何在touch_screen上面显示出来的
		}
		break;
		case 4:
		//从右到左：
		for(x=0; x<w; x++)
		{
			for(y=0; y<h; y++)
			{
				*(fbp + (y+y1)*800 + x1 + w - x - 1) = touch_screen_buf[(h-1-y)*w+w - x - 1];			
			}							
			usleep(1000);//时间停顿，显示刷的效果				
		}
		break;
		//横百叶窗
		case 5:
		//int k;
		for(x=0;x<(w/8);x++)
		{
			for(y=0;y<h;y++)
			{
				for(k=0;k<8;k++)
				{
					*(fbp+800*(y+y1)+x+x1+k*w/8)= touch_screen_buf[(h-1-y)*w+x+k*w/8];
				}
			}
			usleep(2000);			
		}
	 
		break;
		//竖百叶窗（图片的高需要是8的倍数）
		case 6:
		//int k;
		for(y=0;y<(h/8);y++)
		{
			for(x=0;x<w;x++)
			{
				for(k=0;k<8;k++)
				{
					*(fbp+800*(y+y1+k*h/8)+x+x1)= touch_screen_buf[(h-1-y-k*h/8)*w+x];
				}
			}
			usleep(4000);			
		}
	
		break;
		//圆形收缩
		case 7:
		//int k;
		for(k=h;k>=0;k-=10)
		{
			for(y=0;y<h;y++)
			{
				for(x=0;x<w;x++)
				{
					if((x-400)*(x-400)+(y-240)*(y-240)>=k*k)
					{
						*(fbp+800*(y+y1)+x+x1)= touch_screen_buf[(h-1-y)*w+x];
					}
				}
			}
			usleep(1000);
		}
		
		break;
		//圆形扩散
		case 8:
		//int k;
		for(k=0;k<h;k+=10)
		{
			for(y=0;y<h;y++)
			{
				for(x=0;x<w;x++)
				{
					if((x-400)*(x-400)+(y-240)*(y-240)<=k*k)
					{
						*(fbp+800*(y+y1)+x+x1)= touch_screen_buf[(h-1-y)*w+x];
					}
				}
			}
			usleep(1000);
		}
		break;
		//随机填充,拥有雾化的显示效果.但是有bug,只能用于480*800的图片.
		case 9:
		{
			int m=800*480;
			srand((unsigned )time(NULL));
			int n=800*480;
			int t,w;
			int a[480*800];
			for (i=0;i<n;i++)
			a[i]=i;
			for (i=0;i<m;i++)
			{
				w=rand()%(n-i)+i;
				t=a[i];
				a[i]=a[w];
				a[w]=t;
				}
			for(i=0;i<480*800;i++)
			{
				*(fbp+(479-a[i]/800)*800+a[i]%800)=touch_screen_buf[a[i]];
				if(i%800==0)
				{
				usleep(1);
				}
			}
		break;
		/* randomize(); */
		}

	}
	

	close(bmp_fd);

	return  (int *)0;
	
}

int welcome()
{
	char *pre="../gif/IMG00";
	char *type = ".bmp";
	char * buf = malloc(100);
	int i,j;
	clearscr(0,0,800,480);
	for(j=0;j<2;j++)
	{
		for(i=0;i<=105;i++)
		{
			bzero(buf,100);
			if(i<10)
				sprintf(buf,"%s00%d%s",pre,i,type);
			if(i<100&&i>=10)
				sprintf(buf,"%s0%d%s",pre,i,type);
			if(i>=100)
				sprintf(buf,"%s%d%s",pre,i,type);
			//_printf("buf:%s\n",buf);
			//show_bmp("../gif/IMG00000.bmp",0,0,0);
			show_bmp(buf,150,112,0);
		}
	}
	free(buf);
	//system_stat=1;
	return 1;
}
void *getxy()
{
	struct input_event buf;
	int x,y;
	while(1)
	{
		x = -1;y = -1;
		while(1)
		{
			read(ts_fd,&buf,sizeof(buf));
			if(buf.type == EV_ABS)
			{
				if(buf.code == ABS_X)
				{
					x = buf.value;
				}
				else if(buf.code == ABS_Y)
				{
					y = buf.value;
				}		
				else if(buf.code == ABS_PRESSURE)
				{
					press=buf.value;
					////_printf("PRESSURE = %d\n",buf.value);
				}
			}
			if(x>=0 && y>=0)
			{
				xpos=x;
				ypos=y;
				break;
			}
		}
	}
	pthread_exit("screen monitor exit!\n");
}

int submit(char *passwd)
{
	if(strcmp(passwd,"123456")==0)
	{
		show_bmp("/mydesk/source/passright.bmp",200,140,0);
		//_printf("login success!\n");
		while(press==0);
		system_stat=1;
		return 1;
	}
	else
	{
		//_printf("login failed!\n");
		show_bmp("/mydesk/source/passwrong.bmp",200,140,0);
		while(press==0);
		show_bmp("/mydesk/source/numkeyboard.bmp",0,0,0);
		usleep(200*1000);
		return -1;
	}
}
int screen_control()
{
	char *passwd=malloc(20);
	char passwdbuf[20];
	bzero(passwdbuf,20);
	bzero(passwd,20);
	int count;
	int xbuf,ybuf;
	short ifshow=0,i;
	show_bmp("/mydesk/source/numkeyboard.bmp",0,0,0);
	//scr_store(0,183,160,280);
	usleep(1000*200);
	while(1)
	{
		while(press==0);
		xbuf=xpos;
		ybuf=ypos;
		//_printf("x:%d y:%d\n",xpos,ypos);
		
		for(i=0;i<strlen(passwd);i++)
		{
			passwdbuf[i]='*';
		}
		if(xbuf>=0&&xbuf<160&&ybuf>180&&ybuf<=280){//按键1
			xbuf=ybuf=0;
			sprintf(passwd,"%s%d",passwd,1);
			coloradd(0,183,160,280,0x00222222);
			while(press!=0);
			//从258,85开始显示字符
			show_bmp("/mydesk/source/numkeyboard.bmp",0,0,0);
			
			if(ifshow==0)Display_characterXX(258,95,strlen(passwd)+1,passwd,4,0);
			else Display_characterXX(258,95,strlen(passwdbuf)+1,passwdbuf,4,0);
			
			//_printf("passwd %s\n",passwd);
		}
		else if(xbuf>=160&&xbuf<320&&ybuf>180&&ybuf<=280){//按键2
			xbuf=ybuf=0;
			sprintf(passwd,"%s%d",passwd,2);
			coloradd(160,180,320,280,0x00222222);
			while(press!=0);
			show_bmp("/mydesk/source/numkeyboard.bmp",0,0,0);
			if(ifshow==0)Display_characterXX(258,95,strlen(passwd)+1,passwd,4,0);
			else Display_characterXX(258,95,strlen(passwdbuf)+1,passwdbuf,4,0);
			//_printf("passwd %s\n",passwd);
		}
		else if(xbuf>=320&&xbuf<480&&ybuf>180&&ybuf<=280){//按键3
			sprintf(passwd,"%s%d",passwd,3);
			xbuf=ybuf=0;
			coloradd(320,180,480,280,0x00222222);
			while(press!=0);
			show_bmp("/mydesk/source/numkeyboard.bmp",0,0,0);
			if(ifshow==0)Display_characterXX(258,95,strlen(passwd)+1,passwd,4,0);
			else Display_characterXX(258,95,strlen(passwdbuf)+1,passwdbuf,4,0);
			//_printf("passwd %s\n",passwd);
		}
		else if(xbuf>=480&&xbuf<640&&ybuf>180&&ybuf<=280){//按键4
			sprintf(passwd,"%s%d",passwd,4);
			xbuf=ybuf=0;
			coloradd(480,180,640,280,0x00222222);
			while(press!=0);
			show_bmp("/mydesk/source/numkeyboard.bmp",0,0,0);
			if(ifshow==0)Display_characterXX(258,95,strlen(passwd)+1,passwd,4,0);
			else Display_characterXX(258,95,strlen(passwdbuf)+1,passwdbuf,4,0);
			//_printf("passwd %s\n",passwd);
		}
		else if(xbuf>=640&&xbuf<800&&ybuf>180&&ybuf<=280){//按键backspace
		    passwd[strlen(passwd)-1]=0;
			xbuf=ybuf=0;
			passwdbuf[strlen(passwdbuf)-1]=0;
			coloradd(640,180,800,280,0x00222222);
			while(press!=0);
			show_bmp("/mydesk/source/numkeyboard.bmp",0,0,0);
			if(ifshow==0)Display_characterXX(258,95,strlen(passwd)+1,passwd,4,0);
			else Display_characterXX(258,95,strlen(passwdbuf)+1,passwdbuf,4,0);
		}
		else if(xbuf>=0&&xbuf<160&&ybuf>280&&ybuf<=380){//按键5
			sprintf(passwd,"%s%d",passwd,5);
			coloradd(0,280,160,380,0x00222222);
			while(press!=0);
			show_bmp("/mydesk/source/numkeyboard.bmp",0,0,0);
			if(ifshow==0)Display_characterXX(258,95,strlen(passwd)+1,passwd,4,0);
			else Display_characterXX(258,95,strlen(passwdbuf)+1,passwdbuf,4,0);
			//_printf("passwd %s\n",passwd);
		}
		else if(xbuf>=160&&xbuf<320&&ybuf>280&&ybuf<=380){//按键6
			xbuf=ybuf=0;
			sprintf(passwd,"%s%d",passwd,6);
			coloradd(160,280,320,380,0x00222222);
			while(press!=0);
			show_bmp("/mydesk/source/numkeyboard.bmp",0,0,0);
			if(ifshow==0)Display_characterXX(258,95,strlen(passwd)+1,passwd,4,0);
			else Display_characterXX(258,95,strlen(passwdbuf)+1,passwdbuf,4,0);
			//_printf("passwd %s\n",passwd);
		}
		else if(xbuf>=320&&xbuf<480&&ybuf>280&&ybuf<380){//按键7
			xbuf=ybuf=0;
			sprintf(passwd,"%s%d",passwd,7);
			coloradd(320,280,480,380,0x00222222);
			while(press!=0);
			show_bmp("/mydesk/source/numkeyboard.bmp",0,0,0);
			if(ifshow==0)Display_characterXX(258,95,strlen(passwd)+1,passwd,4,0);
			else Display_characterXX(258,95,strlen(passwdbuf)+1,passwdbuf,4,0);
			//_printf("passwd %s\n",passwd);
		}
		else if(xbuf>=480&&xbuf<640&&ybuf>280&&ybuf<380){//按键8
			xbuf=ybuf=0;
			sprintf(passwd,"%s%d",passwd,8);
			coloradd(480,280,640,380,0x00222222);
			while(press!=0);
			show_bmp("/mydesk/source/numkeyboard.bmp",0,0,0);
			if(ifshow==0)Display_characterXX(258,95,strlen(passwd)+1,passwd,4,0);
			else Display_characterXX(258,95,strlen(passwdbuf)+1,passwdbuf,4,0);
			//_printf("passwd %s\n",passwd);
		}
		else if(xbuf>=640&&xbuf<800&&ybuf>280&&ybuf<380){//按键chinese
			xbuf=ybuf=0;
			coloradd(640,280,800,380,0x00222222);
			while(press!=0);
			ifshow=!ifshow;
			show_bmp("/mydesk/source/numkeyboard.bmp",0,0,0);
			if(ifshow==0)Display_characterXX(258,95,strlen(passwd)+1,passwd,4,0);
			else Display_characterXX(258,95,strlen(passwdbuf)+1,passwdbuf,4,0);
		}
		
		else if(xbuf>=0&&xbuf<160&&ybuf>380&&ybuf<480){//按键9
			xbuf=ybuf=0;
			sprintf(passwd,"%s%d",passwd,9);
			coloradd(0,380,160,480,0x00222222);
			while(press!=0);
			show_bmp("/mydesk/source/numkeyboard.bmp",0,0,0);
			if(ifshow==0)Display_characterXX(258,95,strlen(passwd)+1,passwd,4,0);
			else Display_characterXX(258,95,strlen(passwdbuf)+1,passwdbuf,4,0);
			//_printf("passwd %s\n",passwd);
		}
		else if(xbuf>=160&&xbuf<320&&ybuf>380&&ybuf<480){//按键0
			xbuf=ybuf=0;
			sprintf(passwd,"%s%d",passwd,0);
			coloradd(160,380,320,480,0x00222222);
			while(press!=0);
			show_bmp("/mydesk/source/numkeyboard.bmp",0,0,0);
			if(ifshow==0)Display_characterXX(258,95,strlen(passwd)+1,passwd,4,0);
			else Display_characterXX(258,95,strlen(passwdbuf)+1,passwdbuf,4,0);
			//_printf("passwd %s\n",passwd);
		}
		else if(xbuf>=320&&xbuf<480&&ybuf>380&&ybuf<480){//按键 
			xbuf=ybuf=0;
			coloradd(320,380,480,480,0x00222222);
			while(press!=0);
			show_bmp("/mydesk/source/numkeyboard.bmp",0,0,0);
		}
		else if(xbuf>=480&&xbuf<800&&ybuf>380&&ybuf<480){//按键done
			xbuf=ybuf=0;
			coloradd(480,380,800,480,0x00222222);
			while(press!=0);
			show_bmp("/mydesk/source/numkeyboard.bmp",0,0,0);
			if(submit(passwd)==1)
			{
				system_stat=1;
				while(press==0);
				return 0;
			}
		}
		if(strlen(passwd)>16)
		{
			//_printf("输入无效!\n");
		}
	}
	free(passwd);
	return 0;
}
int direction()
{
	//返回值中1代表上划,2代表下划,3代表左划,4代表右划,0表示没有滑动
	int xstart,ystart,xend,yend;
	while(press==0);
	if(press!=0){
	xstart=xpos;
	ystart=ypos;
	}
	while(press!=0){
	xend=xpos;
	yend=ypos;
	}
	if(xstart-xend>70)
	{
		return 3;
	}
	else if(xend-xstart>70)
	{
		return 4;
	}
	else if(ystart-yend>70)
	{
		return 1;
	}
	else if(yend-ystart>70)
	{
		return 2;
	}
	else 
		return 0;
}
int coloradd(int x1,int y1,int x2,int y2,int value)
{
	int i,j;
	for(i=y1;i<y2;i++)
	{
		for(j=x1;j<x2;j++)
		{
			*(fbp+800*i+j)+=value;
		}
	}
}
int colorminus(int x1,int y1,int x2,int y2,int value)
{
	int i,j;
	for(i=y1;i<y2;i++)
	{
		for(j=x1;j<x2;j++)
		{
			*(fbp+800*i+j)-=value;
		}
	}
}
int maininterface()
{
	first:while(1)
	{
		clearscr(0,0,800,480);
		//int *buf=show_bmp("/mydesk/source/mainpic.bmp",0,0,9); 
		show_bmp(background_image,0,0,9);
		/* scr_store(0,0,800,480); */
		show_bmp("/mydesk/source/mp3pic.bmp",300,120,0);
		short xbuf,ybuf,i,dir;
		while(press!=0);
		while(press==0);
		while(1)
		{
			if(xpos>=300&&xpos<=300+200&&ypos>=120&&ypos<=320){
				system_stat = 2;
				return 0;
			}
			else
			{
				dir=direction();
				//_printf("dir %d\n",dir);
				if(dir==3)
					goto second;
				else if(dir==4)
					goto last;
			}
		}
	}
	second:while(1)
	{
		clearscr(0,0,800,480);
		//int *buf=show_bmp("/mydesk/source/mainpic.bmp",0,0,9); 
		show_bmp(background_image,0,0,9);
		/* scr_store(0,0,800,480); */
		show_bmp("/mydesk/source/albumpic.bmp",300,165,0);
		short xbuf,ybuf,i,dir;
		while(press!=0);
		while(press==0);
		while(1)
		{

			dir=direction();
			//_printf("dir %d\n",dir);
			if(dir==3)
				goto third;
			else if(dir==4)
				goto first;

			else if(xpos>=300&&xpos<=300+200&&ypos>=165&&ypos<=165+150){
				system_stat = 3;
				return 0;
			}

		}
	}
	third:while(1)
	{
		clearscr(0,0,800,480);
		//int *buf=show_bmp("/mydesk/source/mainpic.bmp",0,0,9); 
		show_bmp(background_image,0,0,9);
		/* scr_store(0,0,800,480); */
		show_bmp("/mydesk/source/drawing300x240.bmp",250,120,0);
		short xbuf,ybuf,i,dir;
		while(press!=0);
		while(press==0);
		while(1)
		{

			dir=direction();
			//_printf("dir %d\n",dir);
			if(dir==3)
				goto last;
			else if(dir==4)
				goto second;

			else if(xpos>=250&&xpos<=250+300&&ypos>=120&&ypos<=120+240){
				system_stat = 4;
				return 0;
			}

		}
	}
	last:while(1)
	{
		clearscr(0,0,800,480);
		//int *buf=show_bmp("/mydesk/source/mainpic.bmp",0,0,9); 
		show_bmp(background_image,0,0,9);
		/* scr_store(0,0,800,480); */
		show_bmp("/mydesk/source/setting300x300.bmp",250,120,0);
		short xbuf,ybuf,i,dir;
		while(press!=0);
		while(press==0);
		while(1)
		{

			dir=direction();
			//_printf("dir %d\n",dir);
			if(dir==3)
				goto first;
			else if(dir==4)
				goto third;

			else if(xpos>=250&&xpos<=250+300&&ypos>=120&&ypos<=120+240){
				system_stat = 5;
				return 0;
			}
		}
	}
	
	return 0;
}
int painting()
{
	clearscr(0,0,800,480);
	//_printf("point start!\n");
	int i,j,xbuf,ybuf;
	while(1)
	{
		show_bmp("/mydesk/source/clearscr80x40.bmp",700,220,0);
		if(exitevent==1)
		{
			system_stat=1;
			return 0;
		}
		xbuf=xpos;ybuf=ypos;
		if(xbuf>700&&xbuf<780&&ybuf>220&&ybuf<240)
			clearscr(0,0,800,480);
		else
			writescr(xbuf-10,ybuf-10,xbuf+10,ybuf+10,0x00ff0000);
			while(press==0);
		//while(press!=0);
		////_printf("x:%d y:%d\n",xpos,ypos);
		//clearscr(xbuf-10,ybuf-10,xbuf+10,ybuf+10);
	}
	return 0;
}
void *showifright()
{
	//_printf("show if right start!\n");
	while(1)
	{
		if(exitevent==1)
			pthread_exit("show if right exit now!\n");
		pthread_testcancel();
		show_bmp("/mydesk/source/YES80x80.bmp",600,400,0);
	}
	
}
int readconfigs()
{
	FILE *fp=fopen("/mydesk/settings.config","r");
	if(fp==NULL){
		//_printf("nothing read!\n%s",strerror(errno));
		return -1;
	}
	char buf[255];
	fread(buf,1,255,fp);
	//_printf("What was read:\t\n%s",buf);
	strcpy(background_image,buf);
	fclose(fp);
	return 0;
}
int writeconfigs(char *imgname)
{
	//_printf("writeconfigs\n");
	//_printf("These were gotten!\n%s",imgname);
	FILE *fp=fopen("/mydesk/settings.config","w+");
	if(fp==NULL)
	{
		//_printf("File open failed!\n %s\n",strerror(errno));
		return -1;
	}
	int ret = fwrite(imgname,1,255,fp);
	////_printf("These were wrote :\n\t%s",imgname);
	fclose(fp);
}
int settings()
{
	clearscr(0,0,800,480);
	coloradd(0,0,800,480,0x00CCCCCC);
	stu *p=bmphead->next;
	int i=0,j=6;
	/* int xstart,xend,minus,ystart,yend; */
	int dir;
	while(1)
	{
		show_bmp("/mydesk/source/settings1_800x68.bmp",0,0,0);
		show_bmp("/mydesk/source/settings2_800x68.bmp",0,68,0);
		while(press==0);
		while(press!=0);
		if(ypos<68)
		{
			colorminus(0,0,800,68,0x33333333);
			usleep(500*1000);
			goto second;
		}
		else if(ypos>68&&ypos<2*68)
		{
			colorminus(0,68,800,68*2,0x33333333);
			usleep(500*1000);
			goto third;
		}
		
	}
	second:while(1)
	{
		pthread_create(&ifright,NULL,showifright,NULL);
		show_bmp(p->value,0,0,j);
		while(1)
		{
			for(j=1;j<=9;j++)
			{
		/* show_bmp("/mydesk/source/YES80x80.bmp",600,400,0); */
				if(xpos>600&&xpos<600+80&&ypos>400&&ypos<=400+80&&p!=bmphead)
				{
					strcpy(background_image,p->value);
					//_printf("setting success!\n");
					writeconfigs(background_image);
				}
				dir=direction();
				if(exitevent==1){
				system_stat=1;
				return 0;
				}
				
				////_printf("x1:%d,x2:%d\n",xstart,xend);
				if(dir==3)
				{//判断左划
					/* xstart=xpos; */
					system("clear");
					//_printf("left slip\n");
					p=p->preview;
					if(p==bmphead)
						p=p->preview;
						//_printf("%s",p->value);
					show_bmp(p->value,0,0,j);
				}
				else if(dir==4)
				{
					/* xstart=ypos; */
					system("clear");
					/* //_printf("x1 %d:x2 %d\n",xstart,xend); */
					//_printf("right slip\n");
					p=p->next;
					if(p==bmphead)
						p=p->next;
						//_printf("%s",p->value);
					show_bmp(p->value,0,0,j);
				}
				if(exitevent==1)
				{
					usleep(1000);
					system_stat=1;
					return 0;
				}
			}
		}
	}
	third:while(1)
	{
		clearscr(0,0,800,480);
		if(ifshowacrossbar==0)
			show_bmp("/mydesk/source/off_200x80.bmp",580,80,0);
		else{
			
			show_bmp("/mydesk/source/on_200x80.bmp",580,80,0);
		}
		Display_characterXX(40,20,strlen("该功能类似于安卓手机上开发者模式中的显示")+1,"该功能类似于安卓手机上开发者模式中的显示",2,0x00ffffff);
		Display_characterXX(40,80,strlen("屏幕坐标,是否开启?")+1,"屏幕坐标是否开启?,",2,0x00ffffff);
		while(press==0);
		while(press!=0);
		if(xpos>580&&xpos<580+200&&ypos>80&&ypos<80+80){
			if(ifshowacrossbar==0){
				pthread_create(&acrossbar,NULL,across,NULL);
				ifshowacrossbar=1;
			}
			else
				ifshowacrossbar=0;
		}
		if(exitevent==1)
		{
			system_stat=1;
			return 0;
		}
	}
}
int interfacemp3()
{
	stu *p=mp3head;//->next;
	show_bmp("/mydesk/source/mp3main.bmp",0,0,0);
	show_bmp("/mydesk/source/pause_0.bmp",340,400,0);
	show_bmp("/mydesk/source/pre_1.bmp",160,400,0);
	show_bmp("/mydesk/source/next_2.bmp",480,400,0);
	int x,y,pflag=0,rflag=0,nflag=0;
	char *command = "madplay ";
	int volumn = -40;
	char *loopmode = "-r ";
	char *background = " </dev/null >/dev/null 2>1 &";
	char *keepon = "killall -CONT madplay &";
	char *pause = "killall -STOP madplay &";
	char *stop = "killall madplay";
	char *quit = "killall -9 madplay";
	char buf[255];
	int pauseflag=0;
	while(1)
	{
		if(exitevent==1){
			system_stat=1;
			return 0;
			//return 0;
		}
		if(xpos>340&&xpos<340+100&&ypos>400&&ypos<400+64 && press!=0)//暂停/继续按钮
		{
			if(pauseflag==0)
			{
				show_bmp("/mydesk/source/pause_1.bmp",340,400,0);
				while(press!=0);
				//_printf("start plating!\n");
				p=p->next;
				sprintf(buf,"madplay %s -A -40 -r </dev/null >/dev/null 2>1 &",p->value);
				system(buf);
				pauseflag=pauseflag%2+1;
				//continue;
			}
			else if(pauseflag%2==1)
			{
				show_bmp("/mydesk/source/pause_1.bmp",340,400,0);
				while(press!=0);
				system("killall -STOP madplay &");
				//pauseflag++;
				pauseflag=pauseflag%2+1;
			}
			else
			{
				show_bmp("/mydesk/source/pause_2.bmp",340,400,0);
				while(press!=0);
				system("killall -CONT madplay &");
				//pauseflag++;
				pauseflag=pauseflag%2+1;
			}
		}
		else if(xpos>150&&xpos<150+100 && ypos>400&&ypos<400+64 && press!=0){
			p=p->preview;
			if(p==mp3head)p=p->preview;
			show_bmp("/mydesk/source/pre_2.bmp",150,400,0);
			while(press!=0);
			show_bmp("/mydesk/source/pre_1.bmp",150,400,0);
			sprintf(buf,"madplay %s -A -40 -r </dev/null >/dev/null 2>1 &",p->value);
			system("killall -9 madplay");
			system(buf);
		}
		else if(xpos>480&&xpos<480+100 && ypos>400&&ypos<400+64 && press!=0){
			p=p->next;
			if(p==mp3head)p=p->next;
			show_bmp("/mydesk/source/next_1.bmp",480,400,0);
			while(press!=0);
			show_bmp("/mydesk/source/next_2.bmp",480,400,0);
			sprintf(buf,"madplay %s -A -40 -r </dev/null >/dev/null 2>1 &",p->value);
			system("killall -9 madplay");
			system(buf);
		}
		
	}
	return 0;
}
void *across()
{
	//_printf("point start!\n");
	int i,j,xbuf,ybuf;
	while(1)
	{
		if(ifshowacrossbar==0)
			pthread_exit("across exit!\n");
		xbuf=xpos;ybuf=ypos;
		/* writescr(xbuf-10,ybuf-10,xbuf+10,ybuf+10,0x00ff0000); */
		scr_store(0,ybuf-1,800,ybuf+1);
		scr_store(xbuf-1,0,xbuf+1,480);
		writescr(0,ybuf-1,800,ybuf+1,0x00ff0000);
		writescr(xbuf-1,0,xbuf+1,480,0x00ff0000);
		while(press==0);
		//while(press!=0);
		////_printf("x:%d y:%d\n",xpos,ypos);
		scr_restore(xbuf-1,0,xbuf+1,480);
		scr_restore(0,ybuf-1,800,ybuf+1);
		pthread_testcancel();
	}
}
int interfacealbum()
{

	stu *p=bmphead->next;
	int i=0,j=6;
	/* int xstart,xend,minus,ystart,yend; */
	int dir;
	show_bmp(p->value,0,0,j);
	while(1)
	{
		for(j=1;j<=9;j++)
		{
	
			dir=direction();
			if(exitevent==1){
			system_stat=1;
			return 0;
			}
			
			////_printf("x1:%d,x2:%d\n",xstart,xend);
			if(dir==3)
			{//判断左划
				/* xstart=xpos; */
				system("clear");
				//_printf("left slip\n");
				p=p->preview;
				if(p==bmphead)
					p=p->preview;
					//_printf("%s",p->value);
				show_bmp(p->value,0,0,j);
			}
			else if(dir==4)
			{
				/* xstart=ypos; */
				system("clear");
				/* //_printf("x1 %d:x2 %d\n",xstart,xend); */
				//_printf("right slip\n");
				p=p->next;
				if(p==bmphead)
					p=p->next;
					//_printf("%s",p->value);
				show_bmp(p->value,0,0,j);
			}
		}
	}
}
int scr_store(int x1,int y1,int x2,int y2)
{
	int i,j;
	for(i=y1;i<y2;i++)
		for(j=x1;j<x2;j++)
		*(scrbuffer+i*800+j)=*(fbp+i*800+j);
	return 0;
}
int scr_restore(int x1,int y1,int x2,int y2)
{
	int i,j;
	for(i=y1;i<y2;i++)
		for(j=x1;j<x2;j++)
		*(fbp+i*800+j)=*(scrbuffer+i*800+j);
	return 0;
}
void *bottommenu()
{
		int ystart=0,yend=0;
		int i = 0;
		int scrbuf[60*800];
	while(1)
	{
		//while(ystart!=yend&&press==0);
		if(press!=0)
		ystart=ypos;
		//while(press==200);
		yend=ypos;
		////_printf("event:%d %d\n",ystart,yend);
		if(yend-ystart>20){
			//_printf("slip down event%d%d\n",ystart,yend);
			ystart=yend;
/* 			lseek(lcd_fd,800*419*4,SEEK_SET);
			read(lcd_fd,scrbuf,60*800*4); */
/* 			lseek(lcd_fd,0,SEEK_SET); */
			scr_store(0,419,800,480);
			show_bmp("/mydesk/source/bottommenu.bmp",0,419,0);
			usleep(1000*1000);
			scr_restore(0,419,800,480);

		}

	}
	pthread_exit("bottommenu exit!\n");
}

void *exitpic()
{
	//_printf("exitbuttom already!\n");
	while(1)
	{
		if(system_stat>1)
		{	
			exitevent=0;
			show_bmp("/mydesk/source/exitbutton.bmp",800-24,0,0);
			if(xpos>=800-24&&xpos<=800 && ypos>=0&&ypos<=24 && press!=0)
			{
				exitevent=1;
				show_bmp("/mydesk/source/exitbutton2.bmp",800-24,0,0);
				usleep(300*1000);
			}
			
		}
	}
	pthread_exit("bottommenu exit!\n");
}

pthread_init()
{
	int res = pthread_create(&first_pthread,NULL,getxy,NULL);
	res = pthread_create(&exitbutton,NULL,exitpic,NULL);
	res = pthread_create(&acrossbar,NULL,across,NULL);
	system_stat=1;
		//res = pthread_create(&bottompth,NULL,bottommenu,NULL);
}

int main(int argc,char **argv)
{
	if(argc!=2){
		//_printf("invalid arguments!\n");
		return -1;
	}
	system_stat=0;
	bmphead=init_list();
	mp3head=init_list();
	readconfigs();
/* 	screen_init();
	show_bmp("../gif/IMG00000.bmp",150,112,0);
	getchar(); */
	while(1)
	{
		switch(system_stat)
		{
			case 0://前期系统初始化
				Init_Lcd();
				screen_init();
				//maininterface();
				//welcome();
				pthread_init();
				//screen_control();
				dirsol(argv[1]);
				break;
			case 1://主界面
				//clearscr();
				//showbmp("/mydesk/source/bmp.bmp","",8,NULL);
				maininterface();
				break;
			case 2://mp3
				//_printf("mp3\n");
				interfacemp3();
				break;
			case 3://album
				//_printf("album\n");
				interfacealbum();
				break;
			case 4:
				//_printf("painting!\n");
				painting();
			case 5:
				//_printf("settings!\n");
				settings();
		}
	}

	munmap(fbp,800*480*4);
	UnInit_Lcd();
	free(scrbuffer);
	close(ts_fd);


	return 0;
}