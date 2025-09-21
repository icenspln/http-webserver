// HTTP interpreter functions

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int http_req_parser(char *client_buff, char *method, char *path,
                    char **headers);
void http_fatal(char *message);

char *http_interpreter(char *client_buff) {
  char *method;
  char *path;
  char http_version = '1';
  char **headers;

  // PARSER FUNCTION


//   http_req_parser(client_buff, method, path, headers);

//   if (method == NULL || path == NULL) {
//     http_fatal("Parsing http request failed");
//   }

//   printf("\n[DEBUG] method:%s", method);
//   printf("\n[DEBUG] path:%s", path);

  return NULL;
}

int http_req_parser(char *client_buff, char *method, char *path,
                    char **headers) {
  int counter = 0;
  char *tok;
  char *req_line;

  tok = strtok(client_buff, "\r\n");

  // loop until end of lines.
  while (tok != NULL) {
    counter++;
    if (counter == 1)
      req_line = tok;
    if (counter > 1) {
      // disreguard headers for now..
    }

    tok = strtok(NULL, "\r\n");
  }
  printf("\n[DEBUG] req_line:%s", req_line);

  // parse req_line
  method = strtok(req_line, " ");
  path = strtok(NULL, " ");

  if (method == NULL || path == NULL) {
    http_fatal("Parsing http request failed");
  }


  return 0; // success
}

void http_fatal(char *message) {
  printf("[FATAL] [HTTP]: %s", message);
  exit(-1);
}