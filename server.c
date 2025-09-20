#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define PORT 4000

void fatal(char *message);
int new_connection_handler(int fd);

int main() {

  int socket_fd;
  struct sockaddr_in sockaddr;
  u_int addr_len = sizeof(struct sockaddr);

  sockaddr.sin_family = AF_INET;
  sockaddr.sin_port = htons(PORT);
  sockaddr.sin_addr.s_addr = 0;

  socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_fd == -1) {
    fatal("socket creation failed");
  }

  if (-1 == bind(socket_fd, (struct sockaddr *)&sockaddr, sizeof(sockaddr)))
    fatal("binding socket failed");

  printf("\n[DEBUG] Listening...");

  if (-1 == listen(socket_fd, 8))
    fatal("listening on socket failed");


  // accept loop
  while (1) {
    printf("\n[DEBUG] inside accept loop...");
    new_connection_handler(socket_fd);
  }

  // cleanup
  close(socket_fd);
}

int new_connection_handler(int fd) {
  int socket_fd_peer;
  u_int addr_len = sizeof(struct sockaddr);

  struct sockaddr_in sockaddr_peer;

  if ((socket_fd_peer =
           accept(fd, (struct sockaddr *)&sockaddr_peer, &addr_len)) == -1)
    fatal("accepting connection on socket failed");

  printf("\nNew connection from: %s port %d",
         inet_ntoa(sockaddr_peer.sin_addr), ntohs(sockaddr_peer.sin_port));

  close(socket_fd_peer);
  return 1;
}

void fatal(char *message) {
  printf("%s", message);
  exit(1);
}