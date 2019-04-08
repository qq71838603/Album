#include "my.h"

//浏览相册
int Picture_show(Bmplist bmphead)
{
	int ret, flag=0, count=0;  //标志位
	choice = 2;
	int type = 1;    //特效类型
	char backton[255] = {"/lcd/back.bmp"};      //返回按键图片
	char delbon[255] = {"/lcd/delete.bmp"};     //删除按键图片
	char selbon[255] = {"/lcd/yesorno.bmp"};    //确认删除按键图片
	char limit[255] = {"/lcd/limit.bmp"};       //缩小按键图片
	char ground[255] = {"/lcd/background.bmp"}; //背景白色图片
	char expand[255] = {"/lcd/expand.bmp"};     //放大按键图片
	//读取图片链表，显示第一张图片
	Bmplist pos = malloc(sizeof(bmplist));
	Bmplist q = malloc(sizeof(bmplist));    //指向最后一个节点
	Bmplist p = malloc(sizeof(bmplist));    //作为中间变量删除图片
	
	//开始循环遍历
	while(1)
	{	
		list_for_each_entry(pos, &(bmphead->mybmp), mybmp)
		{ 	
			if(flag == 1)
			{
				count = 0;   //下一张图片重新开始缩放
				printf("当前显示的图片是%s\n,",pos->bmp);
				show_picture(pos->bmp, type);
rebutton:
				show_special_picture(backton, 0);     //显示返回按键图片
				show_special_picture(delbon, 1);      //显示删除按键图片
				show_special_picture(limit, 3);       //显示缩小按键
				show_special_picture(expand, 6);      //显示放大按键
				//每显示一张图片，显示一种类型
				type++;
				if(type == 10)
					type = 1;
				ret = get_xy();
				if(ret == 1)  //前一张
				{
					//读取两次是为了下次循环读取到前一张，而不是重复当前的图片
					pos = list_entry((pos)->mybmp.prev, typeof(*pos), mybmp);
					//头节点为空，需指向最末尾一张
					if(pos == bmphead)
					{
						printf("这是头节点\n");
						pos = q;
					}	
					pos = list_entry((pos)->mybmp.prev, typeof(*pos), mybmp);
				}
				else if(ret == 3)  //先退出for循环浏览，接着返回相册子菜单
					break;
				else if(ret == 4)
				{
					//返回5则删除，返回6则取消
					show_special_picture(selbon, 2);
					choice = 5;
					ret = get_xy();
					if(ret == 5)
					{
						type = 1;
						p = pos;
						pos = list_entry((pos)->mybmp.prev, typeof(*pos), mybmp);
						list_del(&(p->mybmp));
					}
					else if(ret == 6)
					{
						pos = list_entry((pos)->mybmp.prev, typeof(*pos), mybmp);  //继续显示当前的图片
						type = 1;
					}
					choice = 2;
				}
				else if(ret == 5)   //图片缩小一倍或者两倍
				{				
					if(count==0)
					{
						show_picture(ground, 1);	
						show_special_picture(pos->bmp, 4);
						count = 1;   
					}	
					else if(pos==p && count==1)  //下一次同一张照片缩小四倍
					{
						show_picture(ground, 1);	
						show_special_picture(pos->bmp, 5);	
						count = 2;
					}
					p = pos;
					goto rebutton;
				}
				else if(ret == 6)   //图片放大一倍或者两倍
				{
					if(pos==p && count==1)
					{
						show_picture(ground, 1);
						show_picture(pos->bmp, 1);	 //由1/2恢复到原样 
						count = 0;
					}	
					if(pos==p && count==2)
					{
						show_picture(ground, 1);
						show_special_picture(pos->bmp, 4);	 //由1/4恢复到1/2 
						count = 1;
					}
					p = pos;
					goto rebutton;
				}
			}
			//获取q,有待其他方法改善
			if((list_entry((pos)->mybmp.next, typeof(*pos), mybmp) == bmphead) && flag==0)
			{
				q = pos;
				flag = 1;
			}
			//从最后一张到第一张之间有头节点，需跳过指向下一个节点
			if(list_entry((bmphead)->mybmp.next, typeof(*pos), mybmp) == bmphead)
				pos = list_entry((bmphead)->mybmp.next, typeof(*pos), mybmp);
		}
		//退出循环则返回相册子菜单
		if(ret == 3)
		{
			system_state = 3;
			break;
		}		
	}
}

