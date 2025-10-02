
#define PORT 4000
#define BUFFER_SIZE 4096 // 4Mib

// so that compiler doesn't cry
struct sockaddr_in;

void fatal(char *message);
int new_connection_handler(int fd);
int client_handler(int client_sock);
void client_connection_logger(struct sockaddr_in *client_addr);