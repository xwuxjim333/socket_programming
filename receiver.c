/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

#include <string.h> 

#include <sys/wait.h>
#include <signal.h>
#include <ctype.h>          
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>

#define PORT 20000 
#define LENGTH 512 
#define BACKLOG 5

void error(const char *msg)
{
	perror(msg);
	exit(1);
}

int main(int argc, char *argv[])
{
    /* Defining Variables */
    int sockfd, newsockfd, clilen;
    char rev_buf[LENGTH];                   // receive buffer
    struct sockaddr_in serv_addr, cli_addr; // server(receiver) addr & client(sender) addr
    int sin_size, n;

    if (argc < 2) {
        fprintf(stderr,"ERROR usage %s [file format]\n", argv[0]);
        exit(1);
    }

    /* Get the Socket file descriptor */
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1 )
       error("ERROR opening socket\n");
	else 
		printf("[Server] Obtaining socket descriptor successfully.\n");

    /* Fill the client socket address struct */
	serv_addr.sin_family = AF_INET;                // Protocol Family
	serv_addr.sin_port = htons(PORT);              // Port number
	serv_addr.sin_addr.s_addr = INADDR_ANY;        // AutoFill local address(127.0.0.1)
	bzero(&(serv_addr.sin_zero), 8);               // Flush the rest of struct

    /* Bind a special Port */
	if( bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(struct sockaddr)) == -1 )
		error("ERROR on binding");
	else 
		printf("[Server] Binded tcp port %d in addr 127.0.0.1 sucessfully.\n",PORT);

    /* Listen remote connect/calling */
	if(listen(sockfd,BACKLOG) == -1) {
		fprintf(stderr, "ERROR: Failed to listen Port. (errno = %d)\n", errno);
		exit(1);
	}
	else
		printf ("[Server] Listening the port %d successfully.\n", PORT);

	int success = 0;
	while(success == 0) {
		sin_size = sizeof(struct sockaddr_in);

		/* Wait a connection, and obtain a new socket file despriptor for single connection */
		if ((newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &sin_size)) == -1) {
		    fprintf(stderr, "ERROR: Obtaining new Socket Despcritor. (errno = %d)\n", errno);
			exit(1);
		}
		else 
			printf("[Server] Server has got connected from %s.\n", inet_ntoa(cli_addr.sin_addr));

		/*Receive File from Client */
        //char file[100] = "/home/xwuxjim333/Desktop/socket_programming/receive.";
        char file[100] = "/home/mimi/Desktop/socket_programming/receive.";
        strcat(file, argv[1]);
        char* fr_name = file;
		FILE *fr = fopen(fr_name, "a");

		if(fr == NULL)
			printf("File %s Cannot be opened file on server.\n", fr_name);
		else {
			bzero(rev_buf, LENGTH); 
			int fr_block_sz = 0;
            int packet = 0;
            int final = 0;

            /* recevice */
			while((fr_block_sz = recv(newsockfd, rev_buf, LENGTH, 0)) > 0) {
			    int write_sz = fwrite(rev_buf, sizeof(char), fr_block_sz, fr);
                final = final+write_sz;
                packet++;

				if(write_sz < fr_block_sz) //recv and write don't match
			        error("File write failed on server.\n"); 

				bzero(rev_buf, LENGTH);

				if (fr_block_sz == 0 || fr_block_sz != 512) //final packet
					break;

			}

			if(fr_block_sz < 0) {
		        if (errno == EAGAIN)
	                printf("recv() timed out.\n");

	            else {
	                fprintf(stderr, "recv() failed due to errno = %d\n", errno);
					exit(1);
	            }
        	}

            printf("packet = %d\n", packet);
            printf("final byte = %d\n", final);
			printf("[Server] Ok!!! Get %s from sender!!!\n", argv[1]);
            printf("[Server] Save as %s\n", fr_name);
            success = 1;
			fclose(fr); 
		}
    	close(sockfd);
        printf("[Server] Shut down.\n");
    }
}
