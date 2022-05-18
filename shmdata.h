#ifndef SHMDATA_H
#define SHMDATA_H
 
#define DATA_SIZE 1024
 
typedef struct share_used_st
{
    int flag;//非0,可读，0，可写
    unsigned char text[DATA_SIZE];//数据
}share_memory;
 
#endif
