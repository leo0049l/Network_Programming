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
  if(argc!=4) 
  {
  cout<<"Format Error: ./sender <receiver_ip> <receiver_port_number> <file_name>"<<endl;
  return 0;
  }
 int sock_fd;
 if((sock_fd=socket(AF_INET,SOCK_DGRAM,0))<0) return 0;
 struct sockaddr_in addr;
 bzero(&addr,sizeof(addr));
 addr.sin_family=AF_INET;
 addr.sin_port=htons(atoi(argv[2]));//port number
 inet_pton(AF_INET,argv[1],&addr.sin_addr);//ip address 
 socklen_t addrlen = sizeof(addr); 

struct sockaddr recaddr;
 socklen_t reclen = sizeof(recaddr);
 /*
 1.read n bytes from file
 2.send to receiver
 3.set a timer <m sec> and wait for ack
   if(receive ack in m sec) go to 1 (read another n bytes)
   else                     go to 2 (trigger retransmission)
 */
   FILE* f;
   struct filedata a[1];
   a[0].id=0;
   char* buffer=new char[2000];
   char* ack=new char[1];
   f=fopen(argv[3],"r");
   int num=fileno(f);
   int n;
   int total=0;
   while(n=read(num,buffer,2000))
   {
   	total+=n;
   } fclose(f); 
   a[0].total=total;
    
   //reopen
   f=fopen(argv[3],"r");
   num=fileno(f);
   int full=0;
   //socket option init
   struct timeval t;
   t.tv_sec=1;
   t.tv_usec=0;
   setsockopt(sock_fd,SOL_SOCKET,SO_RCVTIMEO,&t,sizeof(t)); 
   while(n=read(num,a[0].data,1400))
   {
     cout<<n<<endl; 
     a[0].id++;
     a[0].size=n;
     a[0].flag=0;
     cout<<a[0].id<<endl;
	  while(1)
     {
     sendto(sock_fd, a, sizeof(a), 0,(struct sockaddr*)&addr, addrlen);
      if(recvfrom(sock_fd, ack, 1, 0,&recaddr, &reclen)<0)//ack
      {
       cout<<"timeout"<<endl;
       a[0].flag=1;
       continue;
      }
      else
      {
       cout<<"hi"<<endl;
       break;
      }
     }
   }
   printf("succeeded\n");
  fclose(f); 	
	
  return 0;	
}
