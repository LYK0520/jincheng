#include <stdlib.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "shmdata.h"
#include <string.h>
#include <unistd.h>

/****************************************************************************************
**									共享内存
** shmget():新建共享内存
**	  int shmget(key_t key, size_t size, int shmflg);
** key   ：共享内存关联的标识符
** size  ：新建立的内存大小
** shmflg：权限+创建（如果不存在则创建，存在则打开）
** RETURN VALUE
** 	 A valid segment identifier-shmid, is returned on success, -1 on error.
** shmat():链接写进程到当前共享内存地址空间
**	  void *shmat(int shmid, const void *shmaddr, int shmflg);
** shmid  ：shmget返回的id
** shmaddr：指定共享内存出现在进程内存地址的什么位置，直接指定为NULL,让内核自己决定一个合适的地址位置
** shmflg ：一种只读，另外种读写方式，没有只写的共享内存
** RETURN VALUE
** 	 返回共享的内存地址，否则返回-1
** shmdt():共享内存从当前进程中分离
**	  int shmdt(const void *shmaddr);
** shmaddr：连接的共享内存的起始地址
** RETURN VALUE
** 	 On success shmdt() returns 0; on error -1 is returned
****************************************************************************************/
 
int main()
{
	int running = 1;
	int shmid;								//shmget返回的标识符
	void *shm = NULL;
	struct share_used_st *share = NULL;
	char buffer[BUFSIZ+1];					//用于保存输入的文本  

	
	//创建内存对象
	shmid = shmget((key_t)1234, sizeof(struct share_used_st), 0666|IPC_CREAT);
	if(shmid == -1)
	{
		fprintf(stderr, "Creart a memory object error!!!\n");
		exit(EXIT_FAILURE);
	}
	
	//链接进程与当前共享内存地址空间--返回值是NULL
	shm = shmat(shmid, 0, 0);						//前面0是让OS自动分配地址，后面0是除（#define SHM_RDONLY  010000）都为读写方式 
	if(shm == (void *)-1)							//强制转化为指针0xFFF...,方便移植
	{
		fprintf(stderr,"shmat failed!!!\n");
		exit(EXIT_FAILURE);
	}
	
	printf("Memory attched at %p!!!\n",shm);		//输出内存起始地址
	
	share = (struct share_used_st *)shm;			//把其转化为结构体是为了，实现小协议，对write标识位操作

	while(running)
	{
		if(share->flag == 1)						//按照自己规定的协议，为0才是写，为1只能等待
		{
			sleep(1);
			printf("Wating .....\n");
		}
		printf("Please enter some data:\n");		//输入字符
		
		fgets(buffer, BUFSIZ, stdin);				//获取输入的字符，BUFSIZ是系统常量，为8012
		strncpy(share->text, buffer,DATA_SIZE);		//复制到结构体的text数组中
		
		//写完数据，设置共性内存可读
		share->flag = 1;							//写完。置标志位为读
		
		//输入了end，退出循环（程序）  
		if(strncmp(buffer, "end", 3) == 0)		
		{
			running = 0;
		}
	}

	//把共享内存从当前进程中分离  
	if(shmdt(shm) == -1)
	{
		fprintf(stderr, "Process seperates from memory error!!!\n");
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);
}

