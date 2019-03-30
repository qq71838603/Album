#include "myhead.h"

void slide(int *fx, int *fy)
{
	int count = 0;
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
				*fx = myevent.value;
				count++;
			}
				
			if(myevent.code == ABS_Y)
			{
				*fy = myevent.value;
				count++;
			}

			if(count == 2)
				break;
		}
	}
}