
#define PORT 4000
#define BUFFER_SIZE 5000

void fatal(char *message);
int new_connection_handler(int fd);
char *http_interpreter(char *client_buff);