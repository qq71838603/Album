#include "myhead.h"

extern dl *bmplist;

void album(void)
{
	dl *p = bmplist;
	p = p->next;

	int a = 0;
	int b = 3;
	int ret;

	int a_x, a_y;
	int b_x, b_y;
	char albuf[20];

	display("/share/project1/save/1.bmp", 0, 0, 800, 480);
	while(1)
	{
		slide(&a_x, &a_y);
		get_xy(&b_x, &b_y);
		//printf("%d  %d\n", a_x, a_y);
		//printf("%d  %d\n", b_x, b_y);

		if((b_x-a_x>100) && (b_x-a_x<500)) //上一张
		{
			if(p->prev == bmplist)
			{
				printf("this is first bmp\n");
				display("/share/project1/err/err5.bmp", 200, 190, 400, 100);
				sleep(1);
				showbmp("/share/project1/save/1.bmp");
				continue;
			}
			else
			{
				p = p->prev;
				bzero(albuf, 20);
				sprintf(albuf, "/share/project1/save/%s", p->pathname);
				display2(albuf, 0, 0, 800, 480);
				continue;
			}
		}

		if((a_x-b_x>100) && (a_x-b_x<500)) //下一张
		{
			if(p->next == bmplist)
			{
				printf("this is last bmp\n");
				display("/share/project1/err/err6.bmp", 200, 190, 400, 100);
				sleep(1);
				showbmp("/share/project1/save/9.bmp");
				continue;
			}
			else
			{
				p = p->next;
				bzero(albuf, 20);
				sprintf(albuf, "/share/project1/save/%s", p->pathname);
				display1(albuf, 0, 0, 800, 480);
				continue;
			}
		}

		if((a_y-b_y>100) && (a_y-b_y<500)) //循环
		{
			while(p->next != bmplist)
			{
				p = p->next;
				ret = (rand() % (b-a)) + a;
				switch(ret)
				{
					case 0:
						bzero(albuf, 20);
						sprintf(albuf, "/share/project1/save/%s", p->pathname);
						display1(albuf, 0, 0, 800, 480);
						sleep(1);
						break;
					case 1:
						bzero(albuf, 20);
						sprintf(albuf, "/share/project1/save/%s", p->pathname);
						display2(albuf, 0, 0, 800, 480);
						sleep(1);
						break;
					case 2:
						bzero(albuf, 20);
						sprintf(albuf, "/share/project1/save/%s", p->pathname);
						display3(albuf, 0, 0, 800, 480);
						sleep(1);
						break;
				}
			}	
		}

		if((b_y-a_y>100) && (b_y-a_y<500)) //返回
		{
			init_menu();
		}
	}
}