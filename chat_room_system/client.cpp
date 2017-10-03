#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <algorithm>
#include <iostream>
using namespace std;
int ifexit(char* input)
{
 if(input[0]=='e'&&input[1]=='x'&&input[2]=='i'&&input[3]=='t') return 1;
 for(int i=0;i<strlen(input)-3;i++)
 {
  if(input[i]==' ');
  else if(input[i]!=' '&&input[i]=='e'&&input[i+1]=='x'&&input[i+2]=='i'&&input[i+3]=='t')
  return 1;
  else  
  return 0;
 }
}
int main(int argc,char** argv)
{ 

  int sock_fd;
  if(argc!=3)
  {
   return 0;
  }
  else if((sock_fd=socket(AF_INET,SOCK_STREAM,0))<0)
  {
   return 0;
  }
  struct sockaddr_in addr;
  bzero(&addr,sizeof(addr));
  addr.sin_family=AF_INET;
  addr.sin_port=htons(atoi(argv[2]));
  inet_pton(AF_INET,argv[1],&addr.sin_addr);  

  if(connect(sock_fd,(struct sockaddr *)&addr,sizeof(addr))==-1)
  {
   cout<<"wrong"<<endl; 
   return 0;
  }

  int maxfdp1;
  fd_set r_set;
  FD_ZERO(&r_set);
  while(1)
  {  
  char* input = new char[1024];
  char* output = new char[1024];
    FD_SET(fileno(stdin),&r_set);
    FD_SET(sock_fd,&r_set);
  	maxfdp1=max(fileno(stdin),sock_fd)+1;
  	select(maxfdp1,&r_set,NULL,NULL,NULL);
  	if(FD_ISSET(sock_fd,&r_set))
  	{ 
  	 int len;
  	 if((len=read(sock_fd,input,1024))==0)
  	 {
  	 		close(sock_fd);
			return 0;
  	 }
  	 input[len]='\0';
  	 cout<<input<<endl;
  	}
  	if(FD_ISSET(fileno(stdin),&r_set))
  	{
  		if(fgets(output,1024,stdin)==NULL || ifexit(output)==1)
  		{
  		 close(sock_fd);
  		  return 0;
  		}
  		write(sock_fd,output,strlen(output));
  	}
  	delete input;
  	delete output;
  }
return 0;
}
