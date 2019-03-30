#include "myhead.h"

extern dl *bmplist;
extern dl *mp3list;

dl *init_list()
{
	dl *head = malloc(sizeof(dl));
	head->pathname = malloc(50);
	head->prev = head;
	head->next = head;
	return head;
}

int insert_list(char *path, dl *dllist)
{
	dl *p = dllist;
	dl *new = malloc(sizeof(dl));
	new->pathname = path;
	new->prev = NULL;
	new->next = NULL;

	while(p->next != dllist)
		p = p->next;

	p->next = new;
	new->prev = p;
	new->next = dllist;
	dllist->prev = new;
}

int readdirent(char *path)
{
	char *buf = malloc(50);
	struct dirent *mydir;
	DIR *dirp = opendir(path);

	while((mydir=readdir(dirp)) != NULL)
	{
		if(mydir->d_type == DT_DIR)
		{
			if(strstr(mydir->d_name, ".") != NULL)
				continue;
			else
			{
				bzero(buf, 50);
				sprintf(buf, "%s/%s", path, mydir->d_name);
				readdirent(buf);
			}
		}

		else if(mydir->d_type == DT_REG)
		{
			/*bzero(buf, 50);
			sprintf(buf, "%s/%s", path, mydir->d_name);
			printf("is:%s\n", buf);*/

			if(strstr(mydir->d_name, ".bmp") != NULL)
				insert_list(mydir->d_name, bmplist);

			if(strstr(mydir->d_name, ".mp3") != NULL)
				insert_list(mydir->d_name, mp3list);
		}
	}

	
}