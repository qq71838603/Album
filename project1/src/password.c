#include "myhead.h"

void password(void)
{
	int ps_x, ps_y;
	int i;
	int count = 1;
	int wrong = 0;
	int num[7] = {0};
	int *p = num;
	int *q = num;
	display("/share/project1/password/0.bmp", 0, 0, 800, 480);

	char psbuf1[100];
	char psbuf2[100];

	while(1)
	{
			get_xy(&ps_x, &ps_y);
			
			bzero(psbuf1, 100);
			bzero(psbuf2, 100);
			sprintf(psbuf1, "/share/project1/password/%d.bmp", count);
			sprintf(psbuf2, "/share/project1/password/%d.bmp", count-2);

			if((ps_x>0&&ps_x<266) && (ps_y>100&&ps_y<195)) //输入1
			{
				count++;
				*p = 1;
				showbmp(psbuf1);
				p++;
			}
			else if((ps_x>266&&ps_x<533) && (ps_y>100&&ps_y<195)) //输入2
			{
				count++;
				*p = 2;
				showbmp(psbuf1);
				p++;
			}
			else if((ps_x>533&&ps_x<800) && (ps_y>100&&ps_y<195)) //输入3
			{
				count++;
				*p = 3;
				showbmp(psbuf1);
				p++;
			}
			else if((ps_x>0&&ps_x<266) && (ps_y>195&&ps_y<290)) //输入4
			{
				count++;
				*p = 4;
				showbmp(psbuf1);
				p++;
			}
			else if((ps_x>266&&ps_x<533) && (ps_y>195&&ps_y<290)) //输入5
			{
				count++;
				*p = 5;
				showbmp(psbuf1);
				p++;
			}
			else if((ps_x>533&&ps_x<800) && (ps_y>195&&ps_y<290)) //输入6
			{
				count++;
				*p = 6;
				showbmp(psbuf1);
				p++;
			}
			else if((ps_x>0&&ps_x<266) && (ps_y>290&&ps_y<385)) //输入7
			{
				count++;
				*p = 7;
				showbmp(psbuf1);
				p++;
			}
			else if((ps_x>266&&ps_x<533) && (ps_y>290&&ps_y<385)) //输入8
			{
				count++;
				*p = 8;
				showbmp(psbuf1);
				p++;
			}
			else if((ps_x>533&&ps_x<800) && (ps_y>290&&ps_y<385)) //输入9
			{
				count++;
				*p = 9;
				showbmp(psbuf1);
				p++;
			}
			else if((ps_x>266&&ps_x<533) && (ps_y>385&&ps_y<480)) //输入0
			{
				count++;
				*p = 0;
				showbmp(psbuf1);
				p++;
			}
			else if((ps_x>0&&ps_x<266) && (ps_y>385&&ps_y<480)) //删除
			{
				if(count == 1)
				{
					printf("no number delete\n");
					display("/share/project1/err/err1.bmp", 200, 190, 400, 100);
					sleep(2);
					showbmp("/share/project1/password/0.bmp");
					continue;
				}
				else
				{
					p--;
					*p = 0;
					showbmp(psbuf2);
					count--;
				}
			}
			else if((ps_x>533&&ps_x<800) && (ps_y>385&&ps_y<480)) //确认键
			{
				if(count != 7)
				{
					count = 1;
					p = num;
					printf("not enough number\n");
					display("/share/project1/err/err2.bmp", 200, 190, 400, 100);
					sleep(2);
					showbmp("/share/project1/password/0.bmp");
					continue;
				}
				else
				{
					for(i=0; i<6; i++)
					{
						if(*(q+i) != (i+1))
							break;
					}

					if(i != 6)
					{
						if(wrong == 2)
						{
							display("/share/project1/err/err8.bmp", 200, 190, 400, 100);
							sleep(1);
							display("/share/project1/err/err9.bmp", 0, 0, 800, 480);
							exit(1);
						}
						else if(wrong == 0)
						{
							count = 1;
							p = num;
							printf("password wrong\n");
							display("/share/project1/err/err3.bmp", 200, 190, 400, 100);
							sleep(2);
							showbmp("/share/project1/password/0.bmp");
							wrong++;
							continue;
						}
						else if(wrong == 1)
						{
							count = 1;
							p = num;
							printf("password wrong\n");
							display("/share/project1/err/err4.bmp", 200, 190, 400, 100);
							sleep(2);
							showbmp("/share/project1/password/0.bmp");
							wrong++;
							continue;
						}
					}
					else
						break;
				}
			}
	}
}