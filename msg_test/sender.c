#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#define MSG_FILE "/home/lyk0520/OS/msg/b"



#define BUFFER 255
#define PERM S_IRUSR|S_IWUSR

struct msgbuf
{
    long mtype;
    char mtext[BUFFER+1];
};

char *message[3]= {"I'm Sender,there are some messages for you.",\
                   "This is 1st message.",\
                   "This is 2nd message."
                  };
int main()
{
    struct msgbuf msg;
    key_t key;
    int msgid;
    int i;
    if((key=ftok(MSG_FILE,11))==-1)

    {
        fprintf(stderr,"Creat Key Error：%s \n",strerror(errno));
        exit(EXIT_FAILURE);
    }
    if((msgid=msgget(key,PERM|IPC_CREAT))==-1)
    {
        fprintf(stderr,"Creat MessageQueue Error：%s \n",strerror(errno));
        exit(EXIT_FAILURE);
    }

    for(i=0; i<3; i++)
    {
        msg.mtype=1;
        strncpy(msg.mtext,message[i],BUFFER);
        msgsnd(msgid,&msg,sizeof(struct msgbuf),0);
    }
    memset(&msg,'\0',sizeof(struct msgbuf));
    msgrcv(msgid,&msg,sizeof(struct msgbuf),2,0);
    printf("Sender receive: %s\n",msg.mtext);
    if(msgctl(msgid, IPC_RMID, 0) == -1)
    {
        fprintf(stderr, "Remove MessageQueue Error%s \n",strerror(errno));
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}
