#include "myhead.h"

//获取x,y坐标
void get_xy(int *lx, int *ly)
{
	int ts_fd = open("/dev/input/event0", O_RDONLY);
	if(ts_fd == -1)
	{
		perror("open abs fail\n");
		exit(-1);
	}
	
	struct input_event myevent;
	while(1)
	{
		read(ts_fd, &myevent, sizeof(myevent));
		
		if(myevent.type == EV_ABS)
		{
			
			if(myevent.code == ABS_X)
			{
				*lx = myevent.value;
			}
			if(myevent.code == ABS_Y)
			{
				*ly = myevent.value;
			}
			
			//判断手是否松开
			if(myevent.code==ABS_PRESSURE && myevent.value==0)
			{
				break;
			}
			
		}
	}
	
	close(ts_fd);
}

