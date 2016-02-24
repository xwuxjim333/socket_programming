/* act as client  */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include <string.h>
#include <errno.h> 

#include <sys/wait.h>
#include <signal.h>
#include <ctype.h>          
#include <arpa/inet.h>

#define PORT 20000
#define LENGTH 512 

void error(const char *msg)
{
	perror(msg);
	exit(1);
}

int main(int argc, char *argv[])
{
    /* Variable Definition */
    int sockfd;

    struct sockaddr_in remote_addr; // server(receiver) addr

    if (argc < 2) {
        fprintf(stderr,"usage %s [filename]\n", argv[0]);
       exit(1);
    }

    /* Get the Socket file descriptor */
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        error("ERROR opening socket");

    /* Fill the socket address struct (server)*/
    remote_addr.sin_family = AF_INET;
    remote_addr.sin_port = htons(PORT);
	inet_pton(AF_INET, "127.0.0.1", &remote_addr.sin_addr); 
	bzero(&(remote_addr.sin_zero), 8);
    //bzero((char *) &serv_addr, sizeof(serv_addr));
    //bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);

    /* Try to connect the server */
	if (connect(sockfd, (struct sockaddr *) &remote_addr, sizeof(struct sockaddr)) == -1)
        error("ERROR connecting");
	else 
		printf("[Client] Connected to server at port %d...ok!\n", PORT);

    /* Send File to Server */
    char send_buf[LENGTH];
    //char file[50] = "/home/xwuxjim333/Desktop/socket_programming/";
    char file[50] = "/home/mimi/Desktop/socket_programming/";
    strcat(file, argv[1]);
    char* fs_name = file;
    printf("[Client] Sending %s to the Server... \n", fs_name);

    FILE *fs = fopen(fs_name, "r");
    if(fs == NULL) {
        printf("ERROR: File %s not found.\n", fs_name);
        exit(1);
    }

    bzero(send_buf, LENGTH); 
    int fs_block_sz; 
    int frag = 0;
    int total = 0;

    /* send */
    while((fs_block_sz = fread(send_buf, sizeof(char), LENGTH, fs)) > 0)
    {
        if(send(sockfd, send_buf, fs_block_sz, 0) < 0) {
            fprintf(stderr, "ERROR: Failed to send file %s. (errno = %d)\n", fs_name, errno);
		    break;
		}
        total = total+fs_block_sz;
		bzero(send_buf, LENGTH);
        frag++;
    }
    printf("fragment = %d\n", frag);
    printf("total byte = %d\n", total);
    printf("[Client] File %s sent correctly!!!\n", fs_name);

	close(sockfd);
	printf("[Client] Disconnection\n");
}
