#include "myhead.h"

//定义全局变量
int smallp_x = 550;
int smallp_y = 50;

//声明函数
int choose_menu();
int showbmp(char *bmpname);
int getloca(int *x,int *y);
int get_firstkatoon(char *path);
int showsmallbmp(char *name,int q,int w);
int move();
int up(char *bmpname, int w);

//定义结构体
struct doublelist
{
	char *name;
	struct doublelist *next; //指向下一个节点的指针
	struct doublelist *prev; //指向前面一个节点的指针
};

//初始化双向循环链表
struct doublelist *init_list()
{
	struct doublelist *head = malloc(sizeof(struct doublelist));
	head->next = head;
	head->prev = head;
	return head; 
}

//定义链表
struct doublelist *mp3;
struct doublelist *bmp;
struct doublelist *album;
struct doublelist *music;

//插入数据到链表
int insert_tail(char *path,struct doublelist *mylist)
{
	struct doublelist *p = mylist;
	struct doublelist *newnode = malloc(sizeof(struct doublelist));
	newnode->name = malloc(256);
	strcpy(newnode->name,path);
	newnode->next = NULL;
	newnode->prev = NULL;
	
	//遍历链表，找到尾部
	while(p->next != mylist)
	{
		p = p->next;
	}
	//插入新的数据
	p->next = newnode;
	newnode->prev = p;
	newnode->next = mylist;
	mylist->prev = newnode;
}

//显示链表中的内容
int show_list(struct doublelist *mylist)
{
	struct doublelist *p=mylist;
	//遍历链表，逐一打印
	while(p->next!=mylist)
	{
		p=p->next;
		printf("list:%s\n",p->name);
	}
}

//显示800*480图片
int showbmp(char *bmpname)
{	
	int bmpfd;
	int lcdfd;
	int i;
	char *fbmem;
	char bmpbuf[800*480*3];
	int lcdbuf[800*480]; //int占4字节
	int lcdbuf1[800*480];
	//打开你要显示的800*480大小的bmp图片
	bmpfd=open(bmpname,O_RDWR);
	if(bmpfd==-1)
	{
		printf("open bmp failed!:%s\n",strerror(errno));
		return -1;
	}
	//打开lcd的驱动
	lcdfd=open("/dev/fb0",O_RDWR);
	if(lcdfd==-1)
	{
		printf("open lcd failed!:%s\n",strerror(errno));
		return -1;
	}
	fbmem = mmap(NULL,
			800*480*4,
			PROT_READ | PROT_WRITE,
			MAP_SHARED,
			lcdfd,
			0);
	if(fbmem == (void*)-1)
	{
		printf("mmap failed!reason:%s\n",strerror(errno));
		return -1;
	}
	//读取图片每个像素点的颜色值
	lseek(bmpfd,54,SEEK_SET);//delete head information
	read(bmpfd,bmpbuf,800*480*3); //一口气将所有像素点颜色值全部读取出来存放在数组中
	//将读取的3个字节数据转换:成lcd需要的4个字节
	//bmpbuf[0] bmpbuf[1] bmpbuf[2]每三个一组构成一组BGR  注意bmp存储的时候颜色值的反的
	for(i=0; i<800*480; i++)
		lcdbuf[i]=bmpbuf[3*i]|bmpbuf[3*i+1]<<8|bmpbuf[3*i+2]<<16|0x00<<24;
	                 //00[2][1][0]  
	for(int k = 0; k < 480; k++)
	for(int j = 1; j < 801; j++)
	{
		lcdbuf1[(1 + k)*800 - j] = lcdbuf[(480 - k)*800 - j];
	}
	//将转换之后的颜色值填充到lcd --》写入到lcd
	memcpy(fbmem, lcdbuf1, 800*480*4);

	munmap(fbmem,800*480*4);
	close(bmpfd);
	close(lcdfd);
}

//获取触摸位置
int getloca(int *x,int *y)
{
	int fd;
	int ret;
	int count = 0;//标志位
	struct input_event tsevent;
	//打开触摸屏
	fd = open("/dev/input/event0",O_RDWR);
	if(fd == -1)
	{
		perror("open lcd failed\n");
		return -1;
	}
	//读取坐标
	ret = read(fd, &tsevent, sizeof(struct input_event));
	if(ret == -1)
	{
		printf("something wrong!reason:%s\n",strerror(errno));
		return -1;
	}
	while(1)
	{
		//读取坐标
		read(fd,&tsevent,sizeof(struct input_event));//无法读取到坐标，read会一直阻塞
		//判断事件类型是不是触摸屏事件
		if(tsevent.type == EV_ABS) //说明是触摸屏事件
		{
			//判断x，y坐标
			if(tsevent.code == ABS_X)
			{
				*x=tsevent.value;
				count++;
			}
			if(tsevent.code == ABS_Y)
			{
				*y=tsevent.value;	
				count++;
			}
		
			if(count==2)	break;
		}
	}	
	close(fd);
}

//显示小图片
int showsmallbmp(char *name,int q,int w)
{	
	int bmpfd;
	int lcdfd;
	int p = 0;
	char *fbmem;
	char bmpbuf[200*150*3];
	int lcdbuf[200*150]; //int占4字节
	int lcdbuf1[200*150];
	
	bmpfd=open(name,O_RDWR);
	if(bmpfd==-1)
	{
		printf("open bmp failed!:%s\n",strerror(errno));
		return -1;
	}
	//打开lcd的驱动
	lcdfd=open("/dev/fb0",O_RDWR);
	if(lcdfd==-1)
	{
		printf("open lcd failed!:%s\n",strerror(errno));
		return -1;
	}
	
	fbmem = mmap(NULL,
				800*480*4,
				PROT_READ | PROT_WRITE,
				MAP_SHARED,
				lcdfd,
				0);
	if(fbmem == (void*)-1)
	{
		printf("mmap failed!reason:%s\n",strerror(errno));
		return -1;
	}
	//读取图片每个像素点的颜色值
	lseek(bmpfd,54,SEEK_SET);//delete head information
	read(bmpfd,bmpbuf,200*150*3); //一口气将所有像素点颜色值全部读取出来存放在数组中
	//将读取的3个字节数据转换:成lcd需要的4个字节
	//bmpbuf[0] bmpbuf[1] bmpbuf[2]每三个一组构成一组BGR  注意bmp存储的时候颜色值的反的
	for(int i=0; i<200*150; i++)
		lcdbuf[i]=bmpbuf[3*i]|bmpbuf[3*i+1]<<8|bmpbuf[3*i+2]<<16|0x00<<24;
	                 //00[2][1][0]  
	
	//翻转800*480图片
	for(int k = 0; k < 150; k++)
	for(int j = 1; j < 201; j++)
	{
		lcdbuf1[(1 + k)*200 - j] = lcdbuf[(150 - k)*200 - j];
	}
	
	//将小图片存入内存映射
	for(int j = 0; j < 150; j++)
	{
		memcpy(fbmem + 4 * ((q + w * 800) + 800 * j),&lcdbuf1[p], 200*4);
		p = p + 200;
	}

	//关闭
	munmap(fbmem,800*480*4);
	close(bmpfd);
	close(lcdfd);
}

//拖动小图片
int move()
{
	int x,y;
	getloca(&x,&y);
	smallp_x = x;
	smallp_y = y;
	printf("x = %d, y = %d\n",smallp_x,smallp_y);
	showbmp("test1/music.bmp");
	showsmallbmp("test1/smallphoto.bmp",smallp_x,smallp_y);
}

//左右滑动翻页
int changepage(int f)
{
	int fd;
	int ret;
	int count = 0;
	struct input_event tsevent;
	//打开触摸屏
	fd = open("/dev/input/event0",O_RDWR);
	if(fd == -1)
	{
		perror("open lcd failed\n");
		return -1;
	}
	//读取坐标
	ret = read(fd, &tsevent, sizeof(struct input_event));
	if(ret == -1)
	{
		printf("something wrong!reason:%s\n",strerror(errno));
		return -1;
	}
	while(1)
	{
		//读取坐标
		read(fd,&tsevent,sizeof(struct input_event));//无法读取到坐标，read会一直阻塞
		//判断事件类型是不是触摸屏事件

		if(tsevent.type == EV_ABS) //说明是触摸屏事件
		{		
			if(tsevent.code == ABS_X)
			{	
				f = tsevent.value;
			}
		}
		if(f != -1) break;
	}
	close(fd);
	return f;
}

//播放音乐
int playmusic(struct doublelist *mp3)
{	
	int w;
	int d = -1;
	int s[2] = {0};
	int x, y;
	char n[256] = {0};
	
	music = init_list();

	struct doublelist *p = mp3;
	while(p->next != mp3)
	{
		p = p->next;
		if(strstr(p->name, "test1/mp3") != NULL)
		insert_tail(p->name, music);
	}
	show_list(music);	
	
	showbmp("test1/music.bmp");
	showsmallbmp("test1/smallphoto.bmp",smallp_x,smallp_y);
	system("madplay test1/report/music.mp3");
	music = music->next;
	sprintf(n,"madplay %s",music->name);
	system(n);
	
	while(1)
	{
		getloca(&x,&y);
		if((x > 0 && x < 100) && (y > 0 && y < 60))//back to main menu
		{
			showbmp("test1/main.bmp");
			break;
		}
		if((x > 274 && x < 517) && (y > 133 && y < 350))//play
		{		
			sprintf(n,"madplay %s",music->name);
			system(n);
		}
		if((x > 230 && x < 340) && (y > 410 && y < 480))//pause
		{
			system("killall -STOP madplay");
		}	
		if((x > 474 && x < 557) && (y > 410 && y < 480))//stop
		{
			system("killall -KILL madplay");
		}
		if((x > smallp_x && x < smallp_x + 200) && (y > smallp_y && y < smallp_y + 150))
		{	
			w = 0;
			while(1)
			{
				move();
				w++;
				if(w == 20) break;
			}
		}
		
		//左右滑动切换音乐
		bzero(s,2);
		for(int i = 0; i < 2; i++)
		{
			d = changepage(d);
			printf("d=%d\n",d);
			switch(i)
			{
				case 0:
					s[0] = d;
					d = -1;
					break;
				case 1:
					s[1] = d;
					d = -1;
					break;	
			}
			printf("s1 = %d, s2 = %d\n",s[0],s[1]);
		}
		if(s[0] < s[1])
		{
			system("killall -KILL madplay");
			music = music->next;
			printf("is:%s\n",music->name);
			sprintf(n,"madplay %s",music->name);
			system(n);
		}
		else
		{
			system("killall -KILL madplay");
			music = music->next;
			printf("is:%s\n",music->name);
			sprintf(n,"madplay %s",music->name);
			system(n);
		}	
	}
}

//播放相片升起特效
int up(char *bmpname, int w)
{	
	int bmpfd;
	int lcdfd;
	int i;
	int p = 0;
	char *fbmem;
	char bmpbuf[800*480*3];
	int lcdbuf[800*480]; //int占4字节
	int lcdbuf1[800*480];
	//打开你要显示的800*480大小的bmp图片
	bmpfd=open(bmpname,O_RDWR);
	if(bmpfd==-1)
	{
		printf("open bmp failed!:%s\n",strerror(errno));
		return -1;
	}
	//打开lcd的驱动
	lcdfd=open("/dev/fb0",O_RDWR);
	if(lcdfd==-1)
	{
		printf("open lcd failed!:%s\n",strerror(errno));
		return -1;
	}
	fbmem = mmap(NULL,
			800*960*4,
			PROT_READ | PROT_WRITE,
			MAP_SHARED,
			lcdfd,
			0);
	if(fbmem == (void*)-1)
	{
		printf("mmap failed!reason:%s\n",strerror(errno));
		return -1;
	}
	//读取图片每个像素点的颜色值
	lseek(bmpfd,54,SEEK_SET);//delete head information
	read(bmpfd,bmpbuf,800*480*3); //一口气将所有像素点颜色值全部读取出来存放在数组中
	//将读取的3个字节数据转换:成lcd需要的4个字节
	//bmpbuf[0] bmpbuf[1] bmpbuf[2]每三个一组构成一组BGR  注意bmp存储的时候颜色值的反的
	for(i=0; i<800*480; i++)
		lcdbuf[i]=bmpbuf[3*i]|bmpbuf[3*i+1]<<8|bmpbuf[3*i+2]<<16|0x00<<24;
	                 //00[2][1][0]  
	for(int k = 0; k < 480; k++)
	for(int j = 1; j < 801; j++)
	{
		lcdbuf1[(1 + k)*800 - j] = lcdbuf[(480 - k)*800 - j];
	}
	//将转换之后的颜色值填充到lcd --》写入到lcd

	for(int j = 0; j < 480; j++)
	{
		memcpy(fbmem + 4 * ((0 + w * 800) + 800 * j),&lcdbuf1[p], 800*4);
		p = p + 800;
	}
	
	//关闭
	munmap(fbmem,800*960*4);
	close(bmpfd);
	close(lcdfd);
}

//进入相册
int show_album(struct doublelist *bmp)
{	
	int d = -1;
	int s[2] = {0};
	int x, y;
	album = init_list();
		
	struct doublelist *p = bmp;
	while(p->next != bmp)
	{
		p = p->next;
		if(strstr(p->name, "test1/album") != NULL)
		insert_tail(p->name, album);
	}
	show_list(album);	
	
	
	album = album->next;
	printf("is:%s\n",album->name);
	showbmp(album->name);
	system("madplay test1/report/album.mp3");
	
	while(1)
	{
		getloca(&x,&y);

		if((x > 250 && x < 300) && (y > 0 && y < 50))
		{
			album = album->next;
			printf("is:%s\n",album->name);
			for(int w = 480; w > 0; w--)
			{
				//照片升起特效
				up(album->name, w);
			}
			
		}
		if((x > 464 && x < 500) && (y > 0 && y < 50))
		{
				showbmp("test1/main.bmp");
				break;

		}
		//左右滑动翻页
		bzero(s,2);
		for(int i = 0; i < 2; i++)
		{
			d = changepage(d);
			printf("d=%d\n",d);
			switch(i)
			{
				case 0:
					s[0] = d;
					d = -1;
					break;
				case 1:
					s[1] = d;
					d = -1;
					break;	
			}
			printf("s1 = %d, s2 = %d\n",s[0],s[1]);
		}
		if(s[0] < s[1])
		{
			album = album->prev;
			printf("is:%s\n",album->name);
			showbmp(album->name);
		}
		else
		{
			album = album->next;
			printf("is:%s\n",album->name);
			showbmp(album->name);
		}
	}
}

//进入主菜单
int choose_menu(struct doublelist *mp3, struct doublelist *bmp)
{
	int x;
	int y;
	struct input_event tsevent;
	while(1)
	{
		getloca(&x,&y);
		if((x > 35 && x < 170) && (y > 15 && y < 130))
		{
			system("madplay test1/report/quit.mp3");
			printf("bye bye!\n");
			break;
		}
		if((x > 78 && x < 288) && (y > 377 && y < 477))
		{
			printf("enjoy album\n");
			show_album(bmp);
		}
		if((x > 511 && x < 715) && (y > 377 && y < 443))
		{
			printf("enjoy music\n");
			playmusic(mp3);
		}
	}
}

//进入登录界面
int login(char *logbmp, struct doublelist *mp3, struct doublelist *bmp)
{
	struct input_event tsevent;
	char code[7] = "0";
	char *rcode = "456789";
	int x[7] = {0};
	int y[7] = {0};
	int fd;
	int ret;
	
	showbmp(logbmp);
	
	//打开触摸屏
	fd = open("/dev/input/event0",O_RDWR);
	if(fd == -1)
	{
		perror("open lcd failed\n");
		return -1;
	}
	for(int h = 0; h < 56; h++)
	{
		//读取坐标
		ret = read(fd, &tsevent, sizeof(struct input_event));
		if(ret == -1)
		{
			printf("something wrong!reason:%s\n",strerror(errno));
			return -1;
		}
		if(tsevent.type == EV_ABS)
		{
			switch(tsevent.code)
			{
				case ABS_X:
					x[h/8] = tsevent.value;
					printf("(%d, ", x[h/8]);
					break;
				case ABS_Y:
					y[h/8] = tsevent.value;
					printf("%d)\n", y[h/8]);
					break;
			}
		}
	}
	
	//保存输入密码
	for(int l = 0; l < 7; l++)
	{	
		printf("%d,%d\n",x[l],y[l]);

		if(x[l] > 90 && x[l] < 240)
		{
			if(y[l] > 155 && y[l] < 247) 
			{
				code[l] = '1';
			}
			if(y[l] > 250 && y[l] < 335)
			{
				code[l] = '4';
			}
			if(y[l] > 335 && y[l] < 425)
			{
				code[l] = '7';
			}
		}
		if(x[l] > 240 && x[l] < 390)
		{
			if(y[l] > 155 && y[l] < 247)
			{
				code[l] = '2';
			}
			if(y[l] > 250 && y[l] < 335) 
			{
				code[l] = '5';
			}
			if(y[l] > 335 && y[l] < 425)
			{
				code[l] = '8';
			}
		}
		if(x[l] > 390 && x[l] < 540)
		{
			if(y[l] > 155 && y[l] < 247)
			{
				code[l] = '3';
			}
			if(y[l] > 250 && y[l] < 335)
			{
				code[l] = '6';
			}
			if(y[l] > 335 && y[l] < 425)
			{
				code[l] = '9';
			}
		}
		if(x[l] > 540 && x[l] < 680)
		{
			/*
			if(y[l] > 155 && y[l] < 247)
			{
				--l;
				--b;
			}
			*/
			if(y[l] > 250 && y[l] < 335)
			{
				code[l] = '0';
			}
			//判断输入密码是否正确
			if(y[l] > 335 && y[l] < 425) 
			{
				if(strcmp(rcode,code) == 0)
				{
					system("madplay test1/report/true.mp3");
					printf("welcome!%d,%s,%s\n",strcmp(rcode,code), rcode ,code);
					showbmp("test1/main.bmp");
					choose_menu(mp3, bmp);
				}
				if(strcmp(rcode,code) != 0)
				{
					printf("wrong code!%d,%s,%s\n",strcmp(rcode,code), rcode ,code);
					system("madplay test1/report/false.mp3");
					showbmp("test1/wrongcode.bmp");
					sleep(1);
				}
			}
		}
	}
	close(fd);
}

//展示开机动画
int show_firstkatoon(struct doublelist *mylist)
{
	char n[50] = {0};
	struct doublelist *p  = mylist;
	while(p->next != mylist)
	{
		p = p->next;
		sprintf(n,"test1/firstkatoon/%s",p->name);
		showbmp(n);
	}
}
int get_firstkatoon(char *path)
{
	char buf[256];
	struct dirent *mydirent;
	struct doublelist *firstkatoon = init_list();
	DIR *dirp = opendir("test1/firstkatoon");
	if(dirp == NULL)
	{
		printf("open dir failed, reason:%s\n",strerror(errno));
		return -1;
	}
	while((mydirent=readdir(dirp)) != NULL) 
	{
		if(strstr(mydirent->d_name, ".bmp") != NULL)
		{
			insert_tail(mydirent->d_name, firstkatoon);
		}
	}
	closedir(dirp);
	show_firstkatoon(firstkatoon);
}

//递归读取目录所有文件
int rdir(char *dirpath, struct doublelist *mp3, struct doublelist *bmp)
{
	char buf[256];
	char mp3_name[256];
	char bmp_name[256];
	struct dirent *mydirent;
	DIR *dirp = opendir(dirpath);
	if(dirp == NULL)
	{
		printf("open dir failed, reason:%s\n",strerror(errno));
		return -1;
	}
	while((mydirent=readdir(dirp)) != NULL) 
	{
		if(strncmp(mydirent->d_name,".",1) == 0) 
			continue; 
		if(mydirent->d_type == DT_DIR)
		{
			sprintf(buf, "%s/%s", dirpath, mydirent->d_name);
			rdir(buf, mp3, bmp);
		}
		if(strstr(mydirent->d_name, ".mp3") != NULL)
		{
			sprintf(mp3_name, "%s/%s", dirpath, mydirent->d_name);
			insert_tail(mp3_name, mp3);
		}
		if(strstr(mydirent->d_name, ".bmp") != NULL)
		{
			sprintf(bmp_name, "%s/%s", dirpath, mydirent->d_name);
			insert_tail(bmp_name, bmp);
		}
	}
	closedir(dirp);
}

int main(int argc, char **argv)
{
	mp3 = init_list();
	bmp = init_list();
	
	rdir(argv[1], mp3, bmp);				//递归读取目录中的所有文件信息
	
	show_list(mp3);	
	show_list(bmp);
	
	for(int k = 0; k < 2; k++)				//显示开机动画
	get_firstkatoon("test1/firstkatoon");
	system("madplay test1/report/welcome.mp3");
	
	while(1)
	login("test1/login.bmp", mp3, bmp);		//进入登陆界面

	return 0;
}
