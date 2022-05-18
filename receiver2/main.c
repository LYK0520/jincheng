#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <sys/msg.h>
#define MSG_FILE "/home/lyk0520/OS/msg/b"
#define BUFFER 255
#define PERM S_IRUSR|S_IWUSR
struct msgbuf
{
    long mtype;
    char mtext[BUFFER+1];
};

int main()
{
    struct msgbuf msg;
    key_t key;
    int msgid;
    int i;
    char *myask="I'm receiver, 3 messages received from you.";
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
        msgrcv(msgid,&msg,sizeof(struct msgbuf),1,0);
        printf("Receiver receive: %s\n",msg.mtext);
    }
    msg.mtype=2;
    strncpy(msg.mtext,myask,BUFFER);
    //msgsnd(msgid,&msg,sizeof(struct msgbuf),0);

    exit(EXIT_SUCCESS);
}
