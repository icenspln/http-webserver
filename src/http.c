// HTTP interpreter functions

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/http.h"
#include "../include/server.h"

char *http_interpreter(char *client_buff) {
  // input client buffer
  // use strtok to walk through lines
  // calculate and copy each line to heap and return the pointer to req_line,
  // headers, body

  struct HTTP_data http;
  char *req_line;
  char *headers;
  char *body;
  int counter = 0;
  u_int len;

  char *token;

  token = strtok(client_buff, "\r\n");

  while (token != NULL) {
    counter++;
    if (counter == 1) {
      len = strlen(token);
      printf("\n[DEBUG] len size: %d", len);
      // req_line = (char *)malloc(len);
      // strncpy(req_line, token, len);
      req_line = token;
    }
    // printf("\n[DEBUG] printing str tokens: %s", token);
    token = strtok(NULL, "\r\n");
  }

  // parse req_line
  parse_req_line(&http,req_line, len);
  // parse headers
  // parse body

  // routes
  // 
  return NULL;

}


void parse_req_line(struct HTTP_data* http, char *req_line, int size) {
  char *token;

  token = strtok(req_line," ");

  // method
  if (token != NULL) {
    printf("\n[DEBUG] method: %s", token);

    if (strncmp(token, "GET", 3) == 0) {
      http->method = 0;
    } else if (strncmp(token, "POST", 4) == 0) {
      http->method = 1;
    } else if (strncmp(token, "PUT", 3) == 0) {
      http->method = 2;
    } else if (strncmp(token, "PATCH", 5) == 0) {
      http->method = 3;
    } else if (strncmp(token, "DELETE", 6) == 0) {
      http->method = 4;
    } else {
      fatal("HTTP method not supported yet.");
    }
  }



  token = strtok(req_line," ");
  // path
  if (token != NULL) {
    http->path = token;
  }
}