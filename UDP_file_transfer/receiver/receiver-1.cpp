#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <algorithm>
#include <iostream>
#include <signal.h>
#include <errno.h>
using namespace std;
struct filedata
{
    char data[1400];
    int id;
    int size;
    int flag;
    int total;
};
int main(int argc,char** argv)
{
    if(argc!=3)
    {
        cout<<"Format Error: ./sender <port_number> <file_name>"<<endl;
        return 0;
    }
    int sock_fd;
    int id=0;
    struct filedata a[1];
    if((sock_fd=socket(AF_INET,SOCK_DGRAM,0))<0) return 0;
    struct sockaddr_in addr;
    bzero(&addr,sizeof(addr));
    addr.sin_family=AF_INET;
    addr.sin_port=htons(atoi(argv[1]));//port number
    addr.sin_addr.s_addr=htonl(INADDR_ANY);//ip address 
    if(bind(sock_fd,(struct sockaddr *)&addr,sizeof(addr))<0) return 0;
    struct sockaddr recaddr;
    socklen_t reclen = sizeof(recaddr);
    socklen_t addrlen = sizeof(addr);
    char* buffer=new char[2000];
    char* ack=new char[1]; 
    ack[0]='\0';
    // signal(SIGALRM,sig_alarm);

    FILE* f;
    f=fopen(argv[2],"w");
    int fd=fileno(f);
    // cout<<size<<endl;
    int check[5000]={0};
    int totalsize=0;
    while(1)
    {
        int n;
        if((n=recvfrom(sock_fd, a,sizeof(a), 0, &recaddr, &reclen))>=0)
        {
            cout<<a[0].id<<" "<<a[0].size<<" "<<a[0].flag<<endl;
            if(check[a[0].id]==1)
            {
                sendto(sock_fd,ack,1,0,&recaddr, reclen);
                continue;
            }
            check[a[0].id]=1;
            sendto(sock_fd,ack,1,0,&recaddr, reclen);
            a[0].data[a[0].size]='\0';
            totalsize+=a[0].size;
            write(fd,a[0].data,a[0].size);
            cout<<totalsize<<endl;
            if(totalsize==a[0].total) break;
        }
    }	
    sendto(sock_fd,ack,1,0,&recaddr, reclen);//make sure ack didnt get lost
    sendto(sock_fd,ack,1,0,&recaddr, reclen);//so double send
    cout<<"succeed!\n";
    return 0;	
}
