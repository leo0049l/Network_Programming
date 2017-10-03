#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#define LISTENQ 1024
using namespace std;
char* username[FD_SETSIZE];
char* userip[FD_SETSIZE];
unsigned short userport[FD_SETSIZE];
char* parse(char* input)
{
 char* output;
 output=new char [1024];
 int len=0; // output length;
 int separate=0;
 int head; 
 int counter=0;
 if(input[0]==' ') head=1; else head=0;
 for(int i=0;i<strlen(input);i++)
 {
  if(input[i]==' '&& separate==0 && head==0 && counter<3)
  {
   output[len]=' ';
   len++;
   separate=1;
   counter++;
  }
  else if(input[i]!=' ' && counter<3)
  {
   output[len]=input[i];
   len++;
   head=0;
   separate=0;
  }
  else if(counter==3)
  {
   output[len]=input[i];
   len++;
  }
  if(input[i]=='\n')
  {
   output[len]='\0';
   break;
  }
 }
  return output;
}
void leave(int client[],int id,const struct sockaddr_in *clientaddr,int maxi)
{
		char* buffer=new char[1024];
		sprintf(buffer,"[Server] %s is offline.",username[id]);
	 		
		for(int a=0;a<=maxi;a++)
	 	{
	 	 if(client[a]>=0 && a!=id)
	 	 { 
	 	   write(client[a],buffer,strlen(buffer));
	 	 }
	 	}
}
void command(char* input,int client[],int id,const struct sockaddr_in *clientaddr,int maxi)
{   int le=strlen(input)-1;
    input[le]='\0';
	char* first=new char[12];
	char* second=new char[12];
	char* buffer=new char[1024];
	char* buffer1=new char[1024];
	char* old=new char[12];
	char* message=new char[1024];
	strcpy(old,username[id]);
	int t;
	for( t=0;input[t]!=' ' && input[t]!='\0';t++)
	{
	 first[t]=input[t];	
	}first[t]='\0'; 
	if(strcmp(first,"name")==0)
	{   int b;
	 	for(b=5;b<strlen(input);b++)
	 	{
	 	 second[b-5]=input[b];	
	 	}second[b]='\0';
	 	int check=0,same=0,success=0;
	 	for(int a=0;a<strlen(second);a++)
	 	{
	 	  if(second[a]<'A' || second[a]>'z' || (second[a]>'Z' && second[a]<'a'))
		   {
		   	check=1; break;
		   }	
	 	}
	 	for(int a=0;a<=maxi;a++)
	 	{
	 	 if(client[a]>=0 && a!=id)
	 	 { 
	 	    if(strcmp(second,username[a])==0)
	 	    {
	 	 	same=1; break;
	 	    }
	 	 }
	 	}
	 	if(strcmp(second,"anonymous")==0)
	 	{
	 	  	sprintf(buffer,"[Server] ERROR: Username cannot be anonymous.");
	 	  	write(client[id],buffer,strlen(buffer));
	 	}
	 	else if(strlen(second)<2 || strlen(second)>12 || check==1)
	 	{
	 		sprintf(buffer,"[Server] ERROR: Username can only consists of 2~12 English letters.");
	 		write(client[id],buffer,strlen(buffer));
	 	}
	 	else if(same==1)
	 	{
	 		sprintf(buffer,"[Server] ERROR: %s has been used by others.",second);
	 		write(client[id],buffer,strlen(buffer));
	 	}
	 	else
	 	{
	 		success=1;
	 		strcpy(username[id],second);
	 		sprintf(buffer,"[Server] You're now known as %s.",second);
	 		write(client[id],buffer,strlen(buffer));
	 	}
	 	
	 	if(success==1)
	 	{
	 		sprintf(buffer1,"[Server] %s is now known as %s.",old,second);
	 		for(int a=0;a<=maxi;a++)
	 		{
	 		 if(client[a]>=0&&a!=id)
			  write(client[a],buffer1,strlen(buffer1));	
	 		}
	 	}
	}
	else if(!strcmp(first,"who"))
	{

	 	   sprintf(buffer,"[Server] %s %s/%hu ->me\n",username[id],userip[id],userport[id]);
	 	   write(client[id],buffer,strlen(buffer)); 		
		for(int a=0;a<=maxi;a++)
	 	{
         if(client[a]>=0 && a!=id)
	 	 { 
		   sprintf(buffer,"[Server] %s %s/%hu",username[a],userip[a],userport[a]);
	 	   write(client[id],buffer,strlen(buffer));
	 	 }
	 	}
	}
	else if(!strcmp(first,"tell"))
	{
	 	if(!strcmp(username[id],"anonymous"))
	 	{
	 	   sprintf(buffer,"[Server] ERROR: You are anonymous.");
	 	   write(client[id],buffer,strlen(buffer));
	 	   return;
	 	}
	 	int b;
	 	if(strlen(input)>5)
	 	{
		 for(b=5;input[b]!=' ' && input[b]!='\0';b++)
	 	{
	 	 second[b-5]=input[b];
	 	}second[b]='\0';
	 	
	 	 int d=0;
		 for(int c=5+strlen(second)+1;c<strlen(input);c++)
		 {
		  message[d]=input[c]; d++;	
		 }	message[d]='\0';
	    }
	 	if(!strcmp(second,"anonymous"))
	 	{
	 	   sprintf(buffer,"[Server] ERROR: The client to which you sent is anonymous.");
	 	   write(client[id],buffer,strlen(buffer));
	 	   return;
	 	}
	 	for(int a=0;a<=maxi;a++)
	 		{
	 		 if(client[a]>=0&&a!=id)
	 		  {
	 		  	if(!strcmp(username[a],second))
	 		  	{
	 		  	 sprintf(buffer,"[Server] SUCCESS: Your message has been sent.");
				 sprintf(buffer1,"[Server] %s tell you %s",username[a],message);	
	 		  	 write(client[a],buffer1,strlen(buffer1));	
	 		  	 write(client[id],buffer,strlen(buffer));
					return;	
	 		  	}
	 		  }  
	 		}
	  		
	 	sprintf(buffer,"[Server] ERROR: The receiver doesn't exist.");	
	 		  	 write(client[id],buffer,strlen(buffer));
	}
	else if(!strcmp(first,"yell"))
	{
		int b;
		 for(b=5;input[b]!='\0';b++)
	 	{
	 	 message[b-5]=input[b];
	 	}message[b]='\0'; sprintf(buffer,"[Server] %s yell %s",username[id],message);
	 		for(int a=0;a<=maxi;a++)
	 		{
	 		 if(client[a]>=0)
	 		  {

	 		  	 write(client[a],buffer,strlen(buffer));

	 		  	}
	 		  }  
	 						return;	
	}
	else
	{
	sprintf(buffer,"[Server] ERROR: Error command.");	
	write(client[id],buffer,strlen(buffer));	
	}
	
}
void connect_success(int client[],int i,const struct sockaddr_in *clientaddr,int maxi)
{
		strcpy(userip[i],inet_ntoa(clientaddr->sin_addr));
		userport[i] = ntohs(clientaddr->sin_port);
		char* buffer=new char[1024];
		sprintf(buffer,"[Server] Hello, %s! From: %s/%hu",username[i],userip[i],userport[i]);
	    write(client[i],buffer,strlen(buffer));
	    sprintf(buffer,"[Server] Someone is coming!");
	    	for(int a=0;a<=maxi;a++)
	 	{
	 	 if(client[a]>=0 && a!=i)
	 	 { 
	 	   write(client[a],buffer,strlen(buffer));
	 	 }
	 	}
	    
}
int main(int argc,char** argv)
{
  int listenfd,maxfd,maxi,connectfd,sockfd;
  int nready,client[FD_SETSIZE];
  ssize_t n;
  socklen_t clientlen;
  fd_set rset,allset;
  char* input=new char[1024];
  char* after=new char[1024];
  for(int i=0;i<FD_SETSIZE;i++)
  {
  	username[i]=new char [20];
  	strcpy(username[i],"anonymous");
  	userip[i]=new char[30];
  }
  if((listenfd=socket(AF_INET,SOCK_STREAM,0))<0)
  {
   return 0;
  }
  struct sockaddr_in listenaddr;
  struct sockaddr_in clientaddr;
  bzero(&listenaddr,sizeof(listenaddr));
  listenaddr.sin_family=AF_INET;
  listenaddr.sin_addr.s_addr=htonl(INADDR_ANY);
  listenaddr.sin_port=htons(7777);

  if(bind(listenfd,(struct sockaddr *)&listenaddr,sizeof(listenaddr))<0)
  {
   return 0;
  }
  if(listen(listenfd,LISTENQ)<0)
  {
   return 0;
  }
  maxfd=listenfd;
  maxi=-1;
int i;
  for(i=0;i<FD_SETSIZE;i++)
  {
   client[i]=-1; 
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
    		clientlen=sizeof(clientaddr);
    		connectfd = accept(listenfd,(struct sockaddr*)&clientaddr,&clientlen);
   		    
    		for(j=0;j<FD_SETSIZE;j++)
    		{
      		 if(client[j]<0)
      		 {
       		 client[j]=connectfd;
       		 break;
      		 }
    		}
    		connect_success(client,j,&clientaddr,maxi);
    		if(j==FD_SETSIZE)
    		{ 
     		cout<<"too many clients"<<endl;
     		return 0;
    		}
    		FD_SET(connectfd,&allset);
    		if(connectfd>maxfd)
    		{
     		maxfd = connectfd;
    		}
    		if(j>maxi)
    		{
     		maxi=j;
   	        }
    		if(--nready<=0)
    		{
     		continue;
    		}
    	 }
       for(j=0;j<=maxi;j++)
       {
        if((sockfd=client[j])<0)
        {
         continue;
        }
        if(FD_ISSET(sockfd,&rset))
        {
         if((n= read(sockfd,input,1024))==0)
         {
          close(sockfd);
          FD_CLR(sockfd,&allset);
          client[j]=-1;
          leave(client,j,&clientaddr,maxi);
         }
         else
         {
          after=parse(input);
          command(after,client,j,&clientaddr,maxi);
         }
         if(--nready<=0)
          break;
        }
       }
  }
return 0;
}
