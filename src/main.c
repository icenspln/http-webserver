#include <arpa/inet.h>
#include <asm-generic/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include "../include/server.h"


int main() {
  int socket_fd, yes = 1;
  struct sockaddr_in sockaddr = {
      AF_INET,
      htons(PORT),
      0,
  };
  u_int addr_len = sizeof(struct sockaddr);

  if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    fatal("socket creation failed");

  if ((setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int))) ==
      -1)
    fatal("setting socket option failed");

  if (bind(socket_fd, (struct sockaddr *)&sockaddr, sizeof(sockaddr)) == -1)
    fatal("binding socket failed");

  if (-1 == listen(socket_fd, 8)) {
    close(socket_fd);
    fatal("listening on socket failed");
  }
  printf("Server Listening on port %d\n", PORT);

  while (1) {
    new_connection_handler(socket_fd);
  }
}

int new_connection_handler(int fd) {
  int socket_fd_peer;
  u_int addr_len = sizeof(struct sockaddr);
  struct sockaddr_in sockaddr_peer;
  int client_handler_status;

  if ((socket_fd_peer =
           accept(fd, (struct sockaddr *)&sockaddr_peer, &addr_len)) == -1) {
    close(fd);
    fatal("accepting connection on socket failed");
  }

  client_connection_logger(&sockaddr_peer);
  client_handler_status = client_handler(socket_fd_peer);

  if (client_handler_status == -1) {
    printf("[ERROR] Handling client failed!\n");
  }

  close(socket_fd_peer);
  return 1;
}

void fatal(char *message) {
  printf("***[FATAL]: %s***\n", message);
  exit(1);
}

void client_connection_logger(struct sockaddr_in *client_addr) {
  printf("New connection from: %s port %d\n", inet_ntoa(client_addr->sin_addr),
         ntohs(client_addr->sin_port));
}