#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<netdb.h>
#include<unistd.h>
#include<signal.h>
#include<stdio.h>
#include<pthread.h>
#include<string.h>
#include<fcntl.h>
#include<errno.h>
#include<sys/time.h>
#include<stdlib.h>
#include<memory.h>
#include<ifaddrs.h>
#include<net/if.h>
#include<stdarg.h>
#include<resolv.h>
#include<stdbool.h>

#define PORT 5001
#define MAX_CLIENTS 50
#define NAME_LEN 32
#define SIZE 1024

static int flag=0;
int sockfd=0;
char name[NAME_LEN];

void str_overwrite(){
	printf("\r%s", "> ");
	fflush(stdout);
}

void str_trim(char* arr,int length){
	for(int i=0;i<length;i++){
		if(arr[i]=='\n'){
			arr[i]='\0';
			break;
		}
	}
}

void recv_msg_handler(){
	char msg[SIZE]={};
	while(1){
		int receive = recv(sockfd,msg,SIZE,0);
		str_trim(msg,SIZE);
		if(receive>0){
			printf("%s\n",msg);
			str_overwrite();
		}
		else if(receive==0)
			break;
		bzero(msg,SIZE);
	}
}

void send_msg_handler(){
	char buff[SIZE]={};
	char msg[SIZE+NAME_LEN]={};
	while(1){
		str_overwrite();
	  	printf("Press 0 for one to one chat or 1 for groupchat or type exit to exit\n");
		str_overwrite();
		fgets(buff,SIZE,stdin);
		str_trim(buff,SIZE);
		send(sockfd,buff,strlen(buff),0);
		if(strcmp(buff,"0")==0){
			str_overwrite();
			printf("enter the name of the person\n");
			str_overwrite();
			bzero(buff,SIZE);
			fgets(buff,SIZE,stdin);
			str_trim(buff,SIZE);
			send(sockfd,buff,strlen(buff),0);
			bzero(buff,SIZE);
			read(sockfd,buff,SIZE);
			str_trim(buff,SIZE);
			str_overwrite();
			printf("%s\n",buff);
			str_trim(buff,SIZE);
			str_overwrite();
			if(strcmp(buff,"not there")==0)
				continue;
			else{
				bzero(buff,SIZE);
				fgets(buff,SIZE,stdin);
				str_trim(buff,SIZE);
				send(sockfd,buff,strlen(buff),0);
			}
		}
		else if(strcmp(buff,"1")==0){
		   //  strcpy(buff,"all");
		   //  str_trim(buff,SIZE);
		   //  send(sockfd,buff,strlen(buff),0);
		     bzero(buff,SIZE);
		     str_overwrite();
		     printf("enter your message\n");
		     str_overwrite();
		     fgets(buff,SIZE,stdin);
		     str_trim(buff,SIZE);
		     send(sockfd,buff,strlen(buff),0);
		     bzero(buff,SIZE);
		}
	//	bzero(buff,SIZE);
		else{
			send(sockfd,buff,strlen(buff),0);
			flag=1;
			break;
		}
               // else{
	      //		sprintf(msg,"%s: %s",name,buff);
	      //		send(sockfd,msg,strlen(msg),0);
	      //	}
		bzero(buff,SIZE);
		bzero(msg,SIZE+NAME_LEN);
	}
}


int main(int argc , char *argv[]){
	char buff[500];
	printf("enter your name: ");
	fgets(name,NAME_LEN,stdin);
	str_trim(name,strlen(name));
	struct sockaddr_in server;

	sockfd = socket(AF_INET,SOCK_STREAM,0);
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr(argv[1]);
	server.sin_port = htons(PORT);

	// connect to the server
	int conn = connect(sockfd,(struct sockaddr*)&server,sizeof(server));
	if(conn ==-1){
		printf("ERROR: connect\n");
		return EXIT_FAILURE;
	}

	// send the name of client
	send(sockfd,name,NAME_LEN,0);
        printf("===WELCOME TO THE GROUP===\n");

        pthread_t send_msg;
	if(pthread_create(&send_msg,NULL,(void*)send_msg_handler,NULL)!=0){
		printf("ERROR : pthread\n");
		return EXIT_FAILURE;
	}
	pthread_t recv_msg;
	if(pthread_create(&recv_msg,NULL,(void*)recv_msg_handler,NULL)!=0){
		        printf("ERROR : pthread\n");
			return EXIT_FAILURE;
	 }

	while(1){
		if(flag){
		   printf("\nBye\n");
		   break;
		}
	}
	close(sockfd);
		  

	return 0;
}
