#define CRLF "\r\n"

int http_sanity_check(char *msg, int len);

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
  char* content_type;
  char* content_length;
} HTTP_res_t;

// typedef struct {
//   char* name;
//   char* value;
// } header;


int http_req_parser(char *msg, HTTP_req_t *http);
int client_handler(int client_sock);
void clean(char **buf); 