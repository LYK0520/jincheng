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
** shmctl():删除共享内存
**	  int shmctl(int shmid, int cmd, struct shmid_ds *buf);
** shmid  ：shmget返回的id
** cmd    :IPC_RMID：删除这片共享内存
** buf    :pointer to a shmid_ds structure
** RETURN VALUE
** 	 On success shmdt() returns 0; on error -1 is returned
****************************************************************************************/ 

int main()
{
	int running = 1;
	int shmid;																				//shmget返回的标识符
	void *shm = NULL;
	struct share_used_st *share = NULL;
	
	
	
	//创建内存对象
	shmid = shmget((key_t)1234, sizeof(struct share_used_st), 0666|IPC_CREAT);
	if(shmid == -1)
	{
		fprintf(stderr, "Creart a memory object error!!!\n");
		exit(EXIT_FAILURE);
	}
	
	//链接进程与当前地址空间
	shm = shmat(shmid, 0, 0);						//前面0是让OS自动分配地址，后面0是除（#define SHM_RDONLY  010000）都为读写方式 
	if(shm == (void *)-1)							//强制转化为指针0xFFF...,方便移植
	{
		fprintf(stderr,"shmat failed!!!\n");
		exit(EXIT_FAILURE);
	}
	
	printf("Memory attched at %p!!!\n",shm);		//输出内存起始地址
	
	//设置内存共享，并设置可写
	share = (struct share_used_st *)shm;			//把其转化为结构体是为了，实现小协议，对write标识位操作
	//这里解释下，为什么这里在while前面把标志位置写？实际中，read会在后台先运行，这样的话后台的读-等待，写直接进入写数据，这个操作对两个进程都是一样的，即两个进程看到的表示都一样，都共享了啊
	share->flag = 0;								//按照自己规定的协议，为0才是写，为1只能等待
	
	while(running)
	{
		if(share->flag != 0)						//读数据
		{
			printf("You just wrote: %s\n",share->text);
			sleep(rand()%3);
			
			//读取完数据，设置written使共享内存段可写 
			share->flag = 0;						//不然就写不进去了	
			
			if(strncmp(share->text, "end", 3) == 0)
			{
				running = 0;
			}
		}
		else
			sleep(1);
	}
	
	//把共享内存从当前进程中分离  
	if(shmdt(shm) == -1)
	{
		fprintf(stderr, "Process seperates from memory error!!!\n");
		exit(EXIT_FAILURE);
	}
	
	//将共享内存释放
	if(shmctl(shmid, IPC_RMID, 0) == -1)
	{
		fprintf(stderr, "Freeing memory error!!!\n");
		exit(EXIT_FAILURE);
	}
	
	exit(EXIT_SUCCESS); 
}

