/*
操作系统上机题目6：进程通信
1.利用管道实现父子进程的通信
2.使用共享存储区实现两个进程的通信
    shmget()；//创建共享内存。
    shmat();//映射共享内存
    shmdt();//撤销映射
原型：int shmget(Key_t key,int size,int shmflg);
    key:共享内存的键值
    size：共享内存大小
    shmflg：权限位
    返回值：共享内存段标识符

3.使用消息缓冲队列实现client和server的通信


*/

#include <signal.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/msg.h>
#define  SHMKEY  82
#define  SVKEY 75 //定义关键字SVKEY
#define  REQ  1

struct msgform  //消息结构
{
   long mtype; 
   char mtext[250];  //文本长度
}; 

void server() 
{   
   int c_pid,s_pid,msgqid;//client的pid 和 消息队列识别码
   struct msgform msg;
   msgqid=msgget(SVKEY,0777|IPC_CREAT);  //创建 75#消息队列 
   while(1){
        msgrcv(msgqid,&msg,250,1,0);  //接收client进程标识数消息 
        c_pid=*(int *)msg.mtext;  //获得 cilent 进程标识数
        if(msg.mtype==REQ){
            printf("(server):serving for client pid=%d\n",c_pid); 
            break;
        }
   }
   

   msg.mtype=c_pid;  //消息类型为 client 进程标识数
   s_pid = getpid();
   *(int *)msg.mtext=s_pid; //获取 server 进程标识数 
   //printf("ser修改后的 mtype=%d，，mtext=%d\n",(int)msg.mtype,*(int *)msg.mtext); //调试信息
   msgsnd(msgqid,&msg,sizeof(int),0);  //发送消息 
   exit(0); 
} 
void client() 
{ 
   int pid,msgqid;//client的pid 和 消息队列识别码
   struct msgform msg;
   msgqid=msgget(SVKEY,0777);  //打开 75#消息队列
   
   pid=getpid();  //获取client进程标识符
   *(int *)msg.mtext=pid;  //pint指针指向client进程标识符
   msg.mtype=REQ;  //消息类型为 1
   //printf("cli修改后的 mtype=%d，，mtext=%d\n",(int)msg.mtype,*(int *)msg.mtext); //调试信息

   msgsnd(msgqid,&msg,sizeof(int),0);  //发送消息msg入msgqid消息队列
   while(1){
        msgrcv(msgqid,&msg,250,pid,0);  //从队列msgqid接收消息msg 
        if(msg.mtype!=REQ){
            printf("(client):receive reply from pid=%d\n",*(int *)msg.mtext);  //显示 server进程标识数
            exit(0); 
        }
   }
} 


int childpid[10]={-1,-1,-1,-1},status=999;

int main(){

	int flg,fd[2];
	flg = pipe(fd); //创建管道
    if (flg == -1)
        perror("pipe error");

    int  shmid,i,* addr;//用于共享存储区通信

	childpid[0]=getpid();//012分别是父进程和两个子进程的pid

	for(int i=1;i<=2;i++){
		int pid=fork();
		if(pid!=0)
			childpid[i] =pid;
		else{
			childpid[i] =getpid();
			break;
		}
	}
    if(getpid()==childpid[0]){//父进程

    }
    
    if(getpid()==childpid[1]){//子进程1
        //利用管道给父进程发送消息
        close(fd[0]);
        char child[100]="is sending a message to parent !\0";
        write(fd[1],child,strlen(child)+1);

        //共享存储区
        shmid=shmget(SHMKEY,512,0777|IPC_CREAT); /*创建共享存储区*/
        printf("进程%d创建了共享存储区:\n",getpid());
        addr=shmat(shmid,0,0);        /*获取首地址*/
        *addr=-1;
        while (*addr == -1);//阻塞，用于进程同步
        printf("进程%d读取共享存储区:%d\n",getpid(),*addr);
   
        server(); 

    }
   
    if(getpid()==childpid[2]){//子进程2
       //接受来自子进程1 的管道消息
        close(fd[1]);
        printf("%d  ",childpid[1]);
        char msg[100];
        read(fd[0],msg,sizeof(msg));
            printf("%s\n",msg);
        

        //共享存储区
        shmid=shmget(SHMKEY,512,0777);      /*打开共享存储区*/
        addr=shmat(shmid,0,0);           /*获得共享存储区首地址*/
        while(*addr != -1);//阻塞，用于进程同步
        
        int xie = getpid();
        printf("进程%d写入共享存储区:%d\n",getpid(),xie);
        *addr= xie;
      
        client(); 
    }

    shmctl(shmid,IPC_RMID,0);     /*撤消共享存储区，归还资源*/
    return 0;
}
 //system("ipcs  -m");//显示共享内存段的信息
//printf("@@@@@@@@@@@@@@@@@@@@@@");
