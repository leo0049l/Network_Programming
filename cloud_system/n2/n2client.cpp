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
using namespace std;
int which(char* input)
{
 if(input[0]=='G'&&input[1]=='E'&&input[2]=='T') return 1; // 1 for GET
 else if(input[0]=='P'&&input[1]=='U'&&input[2]=='T') return 2;//2 for PUT
 else if(input[0]=='L'&&input[1]=='I'&&input[2]=='S'&&input[3]=='T') return 3;//3 for LIST
 else if(input[0]=='E'&&input[1]=='X'&&input[2]=='I'&&input[3]=='T') return 4;//4 for EXIT
 else return 0;
}
char* name(char* input,int i)
{
 char * n,* a,* b;
 n=strtok(input," ");
 a=strtok(NULL," ");
 b=strtok(NULL," ");
 if(i==1) return b;
 else if(i==2) return a;

}
int main(int argc,char** argv)
{
 int sock_fd;
 if((sock_fd=socket(AF_INET,SOCK_STREAM,0))<0) return 0;
 struct sockaddr_in addr;
 bzero(&addr,sizeof(addr));
 addr.sin_family=AF_INET;
 addr.sin_port=htons(atoi(argv[2]));//port number
 inet_pton(AF_INET,argv[1],&addr.sin_addr);//ip address
 if(connect(sock_fd,(struct sockaddr *)&addr,sizeof(addr))==-1) return 0;
 while(1)
 {
  char* command=new char[1024];
  char* buffer=new char[1024];
  char* copy=new char[1024];
  fgets(command,1024,stdin);
  int t=strlen(command);
  command[t-1]='\0';
  strcpy(copy,command);
  int first=which(command);
  
  write(sock_fd,command,strlen(command));
  
  if(first==1)//GET
  {
   FILE* f;
   char* filename=new char [20];
   strcpy(filename,name(command,1));
   int dfd=open(filename,O_RDWR | O_CREAT,0644);
   int nu;  
   char* getdata=new char[5000000];
   nu=read(sock_fd,getdata,15);
   long long int total=atoll(getdata);
   while(nu=read(sock_fd,getdata,5000000))
   {
	 total-=nu;
     write(dfd,getdata,nu);
	 if(total==0)
	 break;    
   }
   close(dfd);
   sprintf(buffer,"%s succeeded\n",copy);
   cout<<buffer;
  }
  else if(first==2)//PUT
  {
   FILE* f;
   char* filename=new char[20];
   strcpy(filename,name(command,2));
   char* filedata=new char[5000000];
   f=fopen(filename,"r");
   int num=fileno(f);
   int n;
   long long int total=0;
   while(n=read(num,filedata,5000000))
   {
   	total+=n;
   } fclose(f); 
   sprintf(buffer,"%llu",total);
   write(sock_fd,buffer,15);
   //reopen
   f=fopen(filename,"r");
   num=fileno(f);
   int full=0;
   while(n=read(num,filedata,5000000))
   {
   	full+=n;
    write(sock_fd,filedata,n);
   }
   sprintf(buffer,"%s succeeded\n",copy);
   cout<<buffer;
  fclose(f); 
  }
  else if(first==3)//LIST
  {
   while(1)
   {
    char* getdata=new char[1024];
    read(sock_fd,getdata,30);
    int len=strlen(getdata);
    getdata[len]='\0';
    cout<<getdata<<endl;;
    if(!strcmp(getdata,"LIST succeeded"))
    { break;}
   }
   
  }
  else if(first==4)//EXIT
  {
   close(sock_fd);
   return 0;
  } 
 
 }
return 0;
}

