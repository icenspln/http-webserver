#include <stdio.h>
#define CRLF "\r\n"

enum methods {GET, POST, PUT, PATCH, DELETE};


typedef struct {
  char *method;
  char *uri;
  char *version;
} req_line_t;

typedef struct {
  req_line_t req_line;
  char *host;
  char *user_agent;
} HTTP_req_t;

typedef struct {
  char* version;
  char* status;
  char* reason;
} status_line_t;

typedef struct {
  status_line_t status_line;
  char* content_type;
  int content_length;
} HTTP_res_t;


int http_req_parser(char *msg, HTTP_req_t *http_req);
char* http_res_constructor(char *body, long body_len, HTTP_res_t *http_res);
int client_handler(int client_sock);
void clean(char **buf); 
int http_sanity_check(char *msg, int len);
long get_file_size(FILE *fd);
int get_route(HTTP_req_t *http_req);