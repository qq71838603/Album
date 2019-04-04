#include "myhead.h"

extern dl *mp3list;

void music(void)
{
	dl *p = mp3list;
	p = p->next;

	int mu_x, mu_y;
	int flag1 = 0;
	int flag2 = 2;
	char playbuf[40];
	showbmp("/share/project1/jiemian/yinyue1.bmp");
	display("/share/project1/err/err7.bmp", 478, 81, 230, 40);
	
	while(1)
	{
		get_xy(&mu_x, &mu_y);

		if((mu_x>185&&mu_x<265) && (mu_y>380&&mu_y<455)) // (1/0 = 播放/暂停)
		{
			if(flag1 == 0) //第一次播放
			{
				bzero(playbuf, 40);
				sprintf(playbuf, "madplay -a -40 /share/project1/save/%s &", p->pathname);
				showbmp("/share/project1/jiemian/yinyue2.bmp");
				display("/share/project1/err/err7.bmp", 478, 81, 230, 40);
				system(playbuf);
				flag1 = 1;
			}
			else if(flag2 == 1) //继续
			{
				showbmp("/share/project1/jiemian/yinyue2.bmp");
				display("/share/project1/err/err7.bmp", 478, 81, 230, 40);
				system("killall -CONT madplay");
				flag2 = 0;
			}
			else if(flag1==1 || flag2==0) //要暂停
			{
				showbmp("/share/project1/jiemian/yinyue1.bmp");
				display("/share/project1/err/err7.bmp", 478, 81, 230, 40);
				system("killall -STOP madplay");
				flag1 = 2;
				flag2 = 1;
			}
		}

		if((mu_x>110&&mu_x<185) && (mu_y>390&&mu_y<445)) //上一首
		{
			system("killall -KILL madplay");

			if(flag1==2 || flag2==1)
			{
				showbmp("/share/project1/jiemian/yinyue2.bmp");
				display("/share/project1/err/err7.bmp", 478, 81, 230, 40);
			}

			if(p->prev == mp3list)
				p = mp3list->prev;
			else
				p = p->prev;
					
			bzero(playbuf, 40);
			sprintf(playbuf, "madplay -a -40 /share/project1/save/%s &", p->pathname);
			system(playbuf);
		}

		if((mu_x>265&&mu_x<335) && (mu_y>390&&mu_y<445)) //下一首
		{
			system("killall -KILL madplay");

			if(flag1==2 || flag2==1)
			{
				showbmp("/share/project1/jiemian/yinyue2.bmp");
				display("/share/project1/err/err7.bmp", 478, 81, 230, 40);
			}

			if(p->next == mp3list)
				p = mp3list->next;
			else
				p = p->next;
					
			bzero(playbuf, 40);
			sprintf(playbuf, "madplay -a -40 /share/project1/save/%s &", p->pathname);
			system(playbuf);
		}

		if((mu_x>480&&mu_x<700) && (mu_y>80&&mu_y<120)) //返回
		{
			system("killall -KILL madplay");
			init_menu();
		}
	}
}