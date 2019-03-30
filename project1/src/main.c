#include "myhead.h"

dl *bmplist;
dl *mp3list;

//主页初始化
void init_menu(void)
{
	int m_x, m_y;
	display("/share/project1/jiemian/zhuye.bmp", 0, 0, 800, 480);

	while(1)
	{
		get_xy(&m_x, &m_y);
		
		if((m_x>45&&m_x<250) && (m_y>180&&m_y<320)) //相册
			album();
		
		if((m_x>470&&m_x<780) && (m_y>160&&m_y<320)) //音乐
			music();
	}
}

int main(int argc, char *argv[])
{
	int i;

	//读取目录中的bmp和mp3
	bmplist=init_list();
	mp3list=init_list();
	readdirent(argv[1]);

	//开机画面
	char openbuf[30];
	for(i=0; i<30; i++)
	{
		sprintf(openbuf, "/share/project1/opengif/%d.bmp", i);
		showbmp(openbuf);
		usleep(33333);
		bzero(openbuf, 30);
	}

	//密码登录界面
	password();

	//主页
	init_menu();
}