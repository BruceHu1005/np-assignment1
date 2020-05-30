/*
 * @Author: Bruce.hufy 
 * @Date: 2020-05-30 11:11:10 
 * @Last Modified by: Bruce.hufy
 * @Last Modified time: 2020-05-30 11:12:57
 */
#include <stdio.h>
#include <stdlib.h>
/* You will to add includes here */
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <iostream>
#include <cmath>

// Included to get the support library
#include "calcLib.c"



using namespace std;

#define PORT 8000
#define BUFF_LEN 512

void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char *argv[]){
  
  /* Do more magic */
  int listen_fd;//to create socket
	int conn_fd;//to accept connection
	struct sockaddr_in serv_addr;//server receive on this address
	struct sockaddr_in client_addr;//server sends to client on this address
	struct sockaddr_storage their_addr;
	int recv_num;
	int send_num;
	char send_buf[BUFF_LEN] = "TEXT TCP 1.0, Enter OK to start!";
	char recv_buf[BUFF_LEN];
	socklen_t clientAddressLength;

  listen_fd=socket(AF_INET,SOCK_STREAM,0); 	 //create socket
	memset(&serv_addr,0,sizeof(serv_addr)); //initialize the socket addresses
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
	serv_addr.sin_port=htons(PORT);

  bind(listen_fd,(struct sockaddr *)&serv_addr, sizeof(serv_addr));
	listen(listen_fd,1);

  while(1) {
		printf("server wait:\n");
		clientAddressLength=sizeof(client_addr);
		conn_fd=accept(listen_fd,(struct sockaddr*)&client_addr, &clientAddressLength);
   
    //rceive from client
		send_num = send(conn_fd, send_buf, strlen(send_buf), 0);
		if (send_num < 0)
		{
			perror("send error:");
			exit(1);
		}
		recv_num = recv(conn_fd, recv_buf, sizeof(recv_buf), 0);
		if (recv_buf[0] == 'O'&&recv_buf[1] == 'K') {
			recv_buf[recv_num] = '\0';
			printf("server receive %d bytes: %s\n", recv_num, recv_buf);
			initCalcLib();
			char *ptr;
			ptr = randomType(); // Get a random arithemtic operator. 

			double f1, f2, fresult;
			int i1, i2, iresult;

			/* Act differently depending on what operator you got, judge type by first char in string. If 'f' then a float */

			if (ptr[0] == 'f') {
				printf("Float\t");
				f1 = randomFloat();
				f2 = randomFloat();

				/* At this point, ptr holds operator, f1 and f2 the operands. Now we work to determine the reference result. */

				if (strcmp(ptr, "fadd") == 0) {
					fresult = f1 + f2;
				}
				else if (strcmp(ptr, "fsub") == 0) {
					fresult = f1 - f2;
				}
				else if (strcmp(ptr, "fmul") == 0) {
					fresult = f1 * f2;
				}
				else if (strcmp(ptr, "fdiv") == 0) {
					fresult = f1 / f2;
				}
				printf("%s %8.8g %8.8g = %8.8g\n", ptr, f1, f2, fresult);
				char str[100];
				sprintf(str, "%s %8.8g %8.8g", ptr, f1, f2);
				send_num = send(conn_fd, str, strlen(str), 0);
				if (send_num < 0)
				{
					perror("sendto error:");
					exit(1);
				}
				recv_num = recv(conn_fd, recv_buf, sizeof(recv_buf), 0);
				if (recv_num < 0)
				{
					perror("recvfrom error:");
					exit(1);
				}
				recv_buf[recv_num] = '\0';
				printf("server receive %d bytes: %s\n", recv_num, recv_buf);
				
				char grade[20];
				if (fabs(stod(recv_buf)-fresult)<0.0001) {

					sprintf(grade, "%s", "OK");

				}
				else
				{
					sprintf(grade, "%s%8.8g", "ERROR, Right answer is ", fresult);
				}
				send_num = send(conn_fd, grade, strlen(grade), 0);
			}
			else {
				printf("Int\t");
				i1 = randomInt();
				i2 = randomInt();

				if (strcmp(ptr, "add") == 0) {
					iresult = i1 + i2;
				}
				else if (strcmp(ptr, "sub") == 0) {
					iresult = i1 - i2;
				}
				else if (strcmp(ptr, "mul") == 0) {
					iresult = i1 * i2;
				}
				else if (strcmp(ptr, "div") == 0) {
					iresult = i1 / i2;
				}

				printf("%s %d %d = %d \n", ptr, i1, i2, iresult);
				char str[100];
				sprintf(str, "%s %d %d", ptr, i1, i2);
				send_num = send(conn_fd, str, strlen(str), 0);
				if (send_num < 0)
				{
					perror("sendto error:");
					exit(1);
				}
				
				recv_num = recv(conn_fd, recv_buf, sizeof(recv_buf), 0);
				recv_buf[recv_num] = '\0';
				printf("server receive %d bytes: %s\n", recv_num, recv_buf);
				
				char grade[20];
				if (abs((stod(recv_buf)) - iresult) < 0.0001) {

					sprintf(grade, "%s", "OK");

				}
				else
				{
					sprintf(grade, "%s%d", "ERROR, Right answer is ", iresult);
				}
				send_num = send(conn_fd, grade, strlen(grade), 0);
				close(conn_fd);
			}

		}
		else {
			char buf[20]="disconnected";
			printf("Client refused connection\n");
			send_num = send(conn_fd, buf, strlen(buf), 0);
			close(conn_fd);
		}

	}
	close(listen_fd);
	return 0;
}
