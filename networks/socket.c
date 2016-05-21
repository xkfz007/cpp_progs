/*
 * This program is the example of socked usage and client and server funtions
 * are listed here. By using different macros during the compiling time, you
 * can get the corresponding exectuable program.
 */
#ifdef _CLIENT_
#include <sys/types.h>
#include <sys/socket.h>										
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>										
#include <arpa/inet.h>										
#include <unistd.h>
#include <fcntl.h>
//#include <time.h>
//#include <string.h>
//the client action
void write_tempfile(const char *filename,char*buffer,size_t length){
    int fd;
    fd=open(filename,O_WRONLY);
    write(fd,&length,sizeof(length));
    write(fd,buffer,length);
    close(fd);
}
int client() {
   int sockfd;												
   int len;													
   struct sockaddr_in address;								
   int result;
     char *buffer="Hello world oooookkk";
     size_t length=strlen(buffer)+1;
   char filename[]="/home/hfz/tmp/temp_file.axx";
   size_t flen=strlen(filename)+1;

   sockfd = socket(AF_INET,SOCK_STREAM, 0);					
   address.sin_family = AF_INET;							
   address.sin_addr.s_addr = inet_addr("127.0.0.1");			 
   address.sin_port = 9734;									  
   len = sizeof(address);
   result = connect(sockfd, (struct sockaddr *) &address, len);	
   if (result == -1) {
      perror("Connection error");
      return 1;
   }
   
   write_tempfile(filename,buffer,length);
   write(sockfd,&flen,sizeof(flen));
   write(sockfd, filename, flen);						
   printf("Message sent to server:%s  \n", filename);
   close(sockfd);										
}
int main() {
    client();
    return 0;
}
#endif
#ifdef _SERVER_
#include <sys/types.h>
#include <sys/socket.h>										
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>										
#include <arpa/inet.h>										
#include <unistd.h>
#include <fcntl.h>
//#include <time.h>
//#include <string.h>
//the server action
char* read_tempfile(const char *filename){
    int fd;
    size_t length;
    char *buffer;
    fd=open(filename,O_RDONLY);
    lseek(fd,0,SEEK_SET);
    read(fd,&length,sizeof(size_t));
    buffer=malloc(length);
    read(fd,buffer,length);
    close(fd);
    return buffer;
}
int server()
{
    char*buffer;
    char filename[200];
    int length;
    int fd;
   int server_sockfd, client_sockfd;						
   int server_len, client_len;								 
   struct sockaddr_in server_address;						  
   struct sockaddr_in client_address;						 
   server_sockfd = socket(AF_INET, SOCK_STREAM, 0);			
   server_address.sin_family = AF_INET;						  
   server_address.sin_addr.s_addr = inet_addr("127.0.0.1");	 
   server_address.sin_port = 9734;							
   server_len = sizeof(server_address);
   bind(server_sockfd, (struct sockaddr *) &server_address, server_len);	
   listen(server_sockfd, 5);									 
   while (1) {
      printf("Server is waiting ....\n");
      client_len = sizeof(client_address);
      client_sockfd = accept(server_sockfd,						
            (struct sockaddr *) &client_address,
            (socklen_t *__restrict) &client_len);
      read(client_sockfd, &length, sizeof(length));	
      read(client_sockfd,filename,length);
      buffer=read_tempfile(filename);
      printf("client message:%s %s\n",filename,buffer);
      free(buffer);
      close(client_sockfd);									
   }
}
int main() {
    server();
    return 0;
}
#endif
