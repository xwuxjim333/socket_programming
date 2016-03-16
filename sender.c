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

#include <unistd.h>

#define PORT 20000
#define LENGTH 512
#define client_ip "10.0.2.15"

void error(const char *msg)
{
	perror(msg);
	exit(1);
}

int main(int argc, char *argv[])
{
    /* Variable Definition */
    int sockfd, newsockfd;
    char send_buf[LENGTH];
    char rev_buf[LENGTH];

    struct sockaddr_in remote_addr; // server(receiver) addr
    struct sockaddr_in client_addr;

    if (argc != 3) {
        fprintf(stderr,"usage %s [filename] [server ip]\n", argv[0]);
       exit(1);
    }

    /* Get the Socket file descriptor for client->server */
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        error("ERROR opening socket");

    /* Fill the socket address struct (server)*/
    remote_addr.sin_family = AF_INET;
    remote_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, argv[2], &remote_addr.sin_addr); 
    bzero(&(remote_addr.sin_zero), 8);

    /* Try to connect the server */
	if (connect(sockfd, (struct sockaddr *) &remote_addr, sizeof(struct sockaddr)) == -1)
        error("ERROR connecting");
	else 
		printf("[Client] Connected to server %s:%d...ok!\n", inet_ntoa(remote_addr.sin_addr), ntohs(remote_addr.sin_port));

    /* Send File to Server */
    //char file[50] = "/home/xwuxjim333/Desktop/socket_programming/";
    char file[50] = "/home/mimi/Desktop/socket_programming/";
    strcat(file, argv[1]);
    char* fs_name = file;
    printf("[Client] Sending %s to the Server\n", fs_name);
    FILE *fs = fopen(fs_name, "r");

    if(fs == NULL) {
        printf("ERROR: File %s not found.\n", fs_name);
        exit(1);
    }

    bzero(send_buf, LENGTH); 
    int fs_block_sz; 
    int frag = 0;
    int total = 0;

    sleep(5);

    /* send */
    while((fs_block_sz = fread(send_buf, sizeof(char), LENGTH, fs)) > 0)
    {
        printf("Packet %d :\n", frag+1);
        int i = 0;
        for(i = 0; i < LENGTH; i++) {
            printf("%x", send_buf[i]);
        }
        printf("\n------------------------------------------------------\n");
        if(send(sockfd, send_buf, fs_block_sz, 0) < 0) {
            fprintf(stderr, "ERROR: Failed to send file %s. (errno = %d)\n", fs_name, errno);
		    break;
		}
        sleep(0.5);
        total = total+fs_block_sz;
		bzero(send_buf, LENGTH);
        frag++;
    }
    printf("fragment = %d\n", frag);
    printf("total byte = %d\n", total);
    printf("[Client] File %s sent correctly!!!\n", fs_name);
    fclose(fs);
    close(sockfd);
    sleep(5);
    printf("[Client]\n");

    /* Get the Socket file descriptor for server->client */
    if ((newsockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        error("ERROR opening socket");

    /* Try to connect the server */
	if (connect(newsockfd, (struct sockaddr *) &remote_addr, sizeof(struct sockaddr)) == -1)
        error("ERROR connecting");
	else 
		printf("[Client] Connected to server %s:%d...ok!\n", inet_ntoa(remote_addr.sin_addr), ntohs(remote_addr.sin_port));

	/* Receive File from Server */
    while(1) {
        printf("[Client] Receiveing file from Server and saving it as back.jpg\n");
        //char* fr_name = "/home/xwuxjim333/Desktop/socket_programming/final.jpg";
        char* fr_name = "/home/mimi/Desktop/socket_programming/back.jpg";
        FILE *fr = fopen(fr_name, "a");

        if(fr == NULL)
            printf("File %s Cannot be opened.\n", fr_name);
    	else {
            bzero(rev_buf, LENGTH); 
            int fr_block_sz = 0;

            while((fr_block_sz = recv(sockfd, rev_buf, LENGTH, 0)) > 0) {
                int write_sz = fwrite(rev_buf, sizeof(char), fr_block_sz, fr);

                if(write_sz < fr_block_sz) 
                    error("File write failed.\n");

                bzero(rev_buf, LENGTH);
            }

            if(fr_block_sz < 0) {
                if (errno == EAGAIN)
				    printf("recv() timed out.\n");

			    else {
				    fprintf(stderr, "recv() failed due to errno = %d\n", errno);
                    exit(1);
                }
            }

            printf("[Client] Ok received from server!\n");
            fclose(fr);
            break;
	    }
    }
    close(newsockfd);

	printf("[Client] Disconnection\n");
}
