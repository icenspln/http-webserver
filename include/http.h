
enum methods { GET, POST, PUT, PATCH, DELETE };

struct HTTP_data {
  int method;
  char *path;
  char *headers;
  char *body;
};

void parse_req_line(struct HTTP_data* http, char* req_line, int size);