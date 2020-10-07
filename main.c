#include <stdio.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "ioct.h"
int main(int argc, char *argv[])
{
       int fd, ret=10;
       unsigned char buffer[50]={};
       if(argc < 2)
       { 
         printf("Usage: /dev/mydev param1.. \n"); 
         return -1;
       }
       // open dev file
       fd=open(argv[1], O_RDWR);
       if(fd < 0)
       {
           printf("open %s failed\n", argv[1]);
           return -1;
       }
       //read from kernel  
       read(fd, buffer, sizeof(buffer));
       printf("%s\n",buffer);
       // set num to 10
       if (ioctl(fd, SETNUM, &ret)< 0)
       {
           printf("set num failed\n");
           return -1;
       }
       if(ioctl(fd, GETNUM , &ret)< 0)
       {
           printf("get num failed\n");
       }
       printf("get default value=%d\n",ret);
       // exchange number ret =atoi(argv[2]);  
       if (ioctl(fd, XNUM, &ret)<0)
       {
           printf("exchange num failed\n");
           return -1;
        }
       printf("get value = %d\n",ret);
       return 0;
} 
/*int main()
{
       
    int num=100 ;
    int num1[]={400,500};    
    int data1=0;    
    int fd;
	printf("123\n");
    fd=open("/dev/ken",O_RDWR);
	if(fd==-1){
		printf("can't open device!\n");
		return -1;
    }
        
    write(fd,&num1,sizeof(num1));
    

    read(fd,&data1,sizeof(data1));
    
    printf("%d\n",data1);
    close(fd);
    return 0;
}*/