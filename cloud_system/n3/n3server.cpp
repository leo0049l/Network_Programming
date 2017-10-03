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
#define LISTENQ 1024
using namespace std;
class cli
{
 public:
 int client;
 int count=0;
 char filename[20][30];
 void setfilename(char* b)
 {
   strcpy(filename[count],b);
   count++;
 }
 void del()
 {
  count=0;	
 }
 void get()
 {  
    char* buffer=new char[30];
 	for(int i=0;i<count;i++)
 	{	
 	 write(client,filename[i],30);	
 	}
	 sprintf(buffer,"LIST succeeded");
     write(client,buffer,30);	
 }
};
cli user[FD_SETSIZE];
void perform(char* input,int sockfd,int i,int maxi)
{	
 if(input[0]=='G'&&input[1]=='E'&&input[2]=='T') // 1 for GET
 {
   char * n= new char[20];char * a= new char [20];char* b= new char[20];
   char* filedata=new char[5000000];
   char* buffer=new char[1024];
   n=strtok(input," ");
   a=strtok(NULL," ");
   b=strtok(NULL," ");
   FILE* f;
   f=fopen(a,"r");
   int num=fileno(f);
   int nu;
   long long int total=0;
   while(nu=read(num,filedata,5000000))
   {
   	total+=nu;
    //write(sock_fd,filedata,strlen(filedata));
   } fclose(f); 
   sprintf(buffer,"%llu",total);
   write(sockfd,buffer,15);
   //reopen
   FILE* g;
   g=fopen(a,"r");
   num=fileno(g);
   while(nu=read(num,filedata,5000000))
   {
    write(sockfd,filedata,nu);
   }
   fclose(g); 
 }
 else if(input[0]=='P'&&input[1]=='U'&&input[2]=='T') //2 for PUT
 {
   char * n= new char[20];char * a= new char [20];char* b= new char[20];
   n=strtok(input," ");
   a=strtok(NULL," ");
   b=strtok(NULL," ");//name
   user[i].setfilename(b);
   int dfd=open(b,O_RDWR | O_CREAT,0644);
   int nu;  
   char* getdata=new char[5000000];
   nu=read(sockfd,getdata,15);
   long long int total=atoll(getdata);
   while(nu=read(sockfd,getdata,5000000))
   {
	 total-=nu;
     write(dfd,getdata,nu);
	 if(total==0)
	 break;    
   }
   close(dfd);
 }
 else if(input[0]=='L'&&input[1]=='I'&&input[2]=='S'&&input[3]=='T') //3 for LIST
 {
   user[i].get();
 }
}
int main(int argc,char** argv)
{
 int listenfd,maxfd,maxi,connectfd,sockfd;
 int nready;
 ssize_t n;
 fd_set rset,allset;
 if((listenfd=socket(AF_INET,SOCK_STREAM,0))<0) return 0;
 struct sockaddr_in listenaddr;
 struct sockaddr_in clientaddr;
 bzero(&listenaddr,sizeof(listenaddr));
 listenaddr.sin_family=AF_INET;
 listenaddr.sin_addr.s_addr=htonl(INADDR_ANY);
 listenaddr.sin_port=htons(7777);
 if(bind(listenfd,(struct sockaddr *)&listenaddr,sizeof(listenaddr))<0) return 0;
 if(listen(listenfd,LISTENQ)<0) return 0;
 maxfd=listenfd;
 maxi=-1;
 for(int i=0;i<FD_SETSIZE;i++)
 {
 user[i].client=-1;
 user[i].count=0;
 }
 FD_ZERO(&allset);
 FD_SET(listenfd,&allset);
 int j;
 while(1)
 {
  rset = allset;
  nready = select(maxfd+1,&rset,NULL,NULL,NULL);
  if(FD_ISSET(listenfd,&rset))
  {
  socklen_t  clientlen=sizeof(clientaddr);
  connectfd= accept(listenfd,(struct sockaddr*)&clientaddr,&clientlen);
  
   for(j=0;j<FD_SETSIZE;j++)
   {
   if(user[j].client<0)
    {
    user[j].client=connectfd;
    break;
    }
   }
   if(j==FD_SETSIZE)
   {
    cout<<"max"<<endl;
    return 0;
   }
   FD_SET(connectfd,&allset);
   if(connectfd,&allset) maxfd=connectfd;
   if(j>maxi) maxi=j;
   if(--nready<=0) continue; 
  }
  for(j=0;j<=maxi;j++)
  {
   if((sockfd=user[j].client)<0) continue;
   if(FD_ISSET(sockfd,&rset))
   {
   	char* input=new char[1024];
    if((n=read(sockfd,input,1024))==0)
    {
     close(sockfd);
     FD_CLR(sockfd,&allset);
     user[j].client=-1;
     user[j].del();
    }
    else
    {	
     perform(input,sockfd,j,maxi);
    }
    if(--nready<=0)
     break;
   }
  }
 }
 return 0;
}

