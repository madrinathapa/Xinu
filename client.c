#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int sock_fd, num_bytes;
    short port_num;
    struct sockaddr_in server_addr;
    struct hostent *server;
    char buffer[256];
    
    if (argc < 3) {
       fprintf(stderr,"Type-- %s hostname port\n", argv[0]);
       exit(0);
    }

    port_num = atoi(argv[2]);
    sock_fd = socket(AF_INET, SOCK_DGRAM, 0);

    if (sock_fd < 0) 
        error("ERROR occured while opening socket");

    server = gethostbyname(argv[1]);

    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host exists\n");
        exit(0);
    }

    bzero((char *) &server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&server_addr.sin_addr.s_addr,
         server->h_length);
    server_addr.sin_port = htons(port_num);

    if (connect(sock_fd,(struct sockaddr *) &server_addr,sizeof(server_addr)) < 0) 
        error("ERROR connecting");

    printf("Please type your message: ");
    bzero(buffer,256);
    fgets(buffer,255,stdin);
    num_bytes = write(sock_fd,buffer,strlen(buffer));
    if (num_bytes < 0) 
         error("Error occured while writing to socket!");
    bzero(buffer,256);
    num_bytes = read(sock_fd,buffer,255);
    if (num_bytes < 0) 
         error("Error while reading from socket!");
    printf("%s\n",buffer);
    close(sock_fd);
    return 0;
}


/*
* http://www.linuxhowtos.org/C_C++/socket.htm
*/