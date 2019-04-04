#include<stdlib.h>
#include<stdio.h>
#include"commonheader.h"

/*
test a double-linked circular list
*/
#include<stdio.h>
#include<stdlib.h>
struct infolist
{
	char value[255];
	struct infolist *next;
	struct infolist *preview;
}*infohead;
#define stu  struct infolist

stu *init_list()
{
	stu *head=malloc(sizeof(stu));
	head->next=head;
	head->preview=head;
	return head;
}
int tail_insert(char *num,stu *mylist)
{
	stu *p = mylist;
	stu *tail = mylist->preview;
	stu *newnode = malloc(sizeof(stu));
	strcpy(newnode->value,num);
	
	newnode->preview=tail;
	newnode->next=p;
	p->preview=newnode;
	tail->next=newnode;
	return 0;
}
int quiery(stu *mylist)
{
	stu *p = mylist;
	do{
		p=p->next;
		printf("%s\n",p->value);
	}while(p!=mylist);
	
}
int backquiery(stu *mylist)
{
	stu *p = mylist;
	do{
		p=p->preview;
		printf("%s\n",p->value);
	}while(p->preview!=mylist);
	
}
//use the function list_for_each_entry th delete a value-specified node
/* int dirsol(char *argv)
{
	struct dirent *dirinfo;
	printf("dir is %s\n",argv);
	DIR *dp=opendir(argv);
	char *tmp=malloc(255);
	if(dp==NULL)
	{
		printf("Error in directory opening!\n");
		return -1;
	}
	char buf[255];
	//读取目录里面的内容
	while((dirinfo = readdir(dp))!=NULL)
	{
		if(strncmp(dirinfo->d_name,".",1)==0)
			continue;
		
		printf("%s\n",dirinfo->d_name);
 		if(dirinfo->d_type==DT_DIR)
		{
			printf("a dir %s\n",dirinfo->d_name);
			sprintf(tmp,"%s%s/",argv,dirinfo->d_name);
			dirsol(tmp);
		}
		
 		if(strstr(dirinfo->d_name,".html")!=NULL)
		{
			insert_mp3list("should insert!",mp3head);
			tail_insert(dirinfo->d_name,infohead);
			printf("I'm here!\n");
		}
		
		else if(strstr(dirinfo->d_name,".bmp")!=NULL)
		{
			
		}
		
	}
	closedir(dp);
	free(tmp);
	return 0;
} */