/*
 * @Author: Bruce.hufy 
 * @Date: 2020-05-30 11:10:48 
 * @Last Modified by: Bruce.hufy
 * @Last Modified time: 2020-05-30 11:14:25
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
#include <string>
#include <sstream>


// Included to get the support library
#include "calcLib.c"

#define PORT "8000"
#define BUFF_LEN 512

using namespace std;

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char *argv[]){
  
  /* Do magic */
  int sock_fd, send_num, recv_num;
  struct addrinfo hints, *servinfo, *p;
  char send_buf[BUFF_LEN];
  char recv_buf[BUFF_LEN];

  if (argc != 2) {
	  fprintf(stderr,"usage: %s hostname (%d)\n",argv[0],argc);
	  exit(1);
	}
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  

  if ((recv_num = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(recv_num));
        return 1;
  }

  for(p = servinfo; p != NULL; p = p->ai_next) {
    if ((sock_fd = socket(p->ai_family, p->ai_socktype,
      p->ai_protocol)) == -1) {
      perror("client: socket");
      continue;
    }

    if (connect(sock_fd, p->ai_addr, p->ai_addrlen) == -1) {
      close(sock_fd);
      perror("client: connect");
      continue;
    }
    break;
  }
  if (p == NULL) {
    fprintf(stderr, "client: failed to connect\n");
    return 2;
  }
  recv_num = recv(sock_fd, recv_buf, sizeof(recv_buf), 0);
  if(recv_num<0 ){
  perror("recvfrom error:");
  exit(1);
  }
  else {
    recv_buf[recv_num] = '\0';
    printf("client receive %d bytes: %s\n", recv_num, recv_buf);
  } 
  
  std::cin >> send_buf;
  send_num=send(sock_fd,send_buf, strlen(send_buf), 0);
  recv_num = recv(sock_fd, recv_buf, sizeof(recv_buf), 0);
  char dis[recv_num];
  sprintf(dis, "%s", "disconnected");
  recv_buf[recv_num] = '\0';
  printf("client receive %d bytes: %s\n", recv_num, recv_buf);
  
  if (strcmp(recv_buf, dis) == 0) {
    close(sock_fd);
    return 0;
  }
  
  string temp = recv_buf;
  string calc[3];
  istringstream is(temp);
  is>>calc[0]>>calc[1]>>calc[2];
  string ptr = calc[0];
  string num1 = calc[1];
  string num2 = calc[2]; 
 
  if(ptr[0]=='f'){
    if (ptr == "fadd")
    {
      sprintf(send_buf, "%8.8g\n", stod(num1) + stod(num2));
    }
    else if (ptr == "fsub")
    {
      sprintf(send_buf, "%8.8g\n", stod(num1) - stod(num2));
    }
    else if (ptr == "fmul")
    {
      sprintf(send_buf, "%8.8g\n", stod(num1) * stod(num2));
    }
    else if (ptr == "fdiv")
    {
      sprintf(send_buf, "%8.8g\n", stod(num1) / stod(num2));
    }
  }
  else{
    if (ptr == "add")
    {
      sprintf(send_buf, "%d\n", stoi(num1) + stoi(num2));
    }
    else if (ptr == "sub")
    {
      sprintf(send_buf, "%d\n", stoi(num1) - stoi(num2));
    }
    else if (ptr == "mul")
    {
      sprintf(send_buf, "%d\n", stoi(num1) * stoi(num2));
    }
    else if (ptr == "div")
    {
      sprintf(send_buf, "%d\n", stoi(num1) / stoi(num2));
    }
  }
  printf("Answer of Client is %s\n", send_buf);
  send_num=send(sock_fd, send_buf, strlen(send_buf), 0);
  recv_num = recv(sock_fd, recv_buf, sizeof(recv_buf), 0);
  recv_buf[recv_num] = '\0';
  printf("client receive %d bytes: %s\n", recv_num, recv_buf);

  close(sock_fd);
  return 0;
}
