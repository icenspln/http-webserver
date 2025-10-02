#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "../include/server.h"
#include "../include/client_handler.h"

int client_handler(int client_sock) {
  char *client_msg;
  int recv_bytes=0;
  HTTP_req_t http_req = {0};

  client_msg = (char *)malloc(BUFFER_SIZE);

  while ((recv_bytes = recv(client_sock, client_msg, BUFFER_SIZE, 0)) != 0) {
    if(recv_bytes == -1) {
      printf("7");
        return -1;
    }

    // terminate client_msg buffer
    client_msg[recv_bytes] = '\0';
    
    // parse http req
    if((http_req_parser(client_msg, &http_req)) == -1) {
      printf("\n[DEBUG] [ERROR] http_req_parser failed");
      goto cleanup;
    }
    // //debug
    printf("\n[DEBUG] logging method & uri");
    printf("\n[DEBUG] uri: %s\n[DEBUG] method: %s\n[DEBUG] version: "
           "%s\n[DEBUG] host: %s\n[DEBUG] user_agent: %s",
           http_req.req_line.uri, http_req.req_line.method,
           http_req.req_line.version, http_req.host, http_req.user_agent);

    

    // create http res

    // routes

    // cleanup
  }

  return 0;

  cleanup:
    printf("\n[ERROR] [DEBUG] cleanup reached");
    clean(&http_req.req_line.method);
    clean(&http_req.req_line.uri);
    clean(&http_req.req_line.version);
    clean(&http_req.host);
    clean(&http_req.user_agent);
    clean(&client_msg);
    return -1;

}

int http_sanity_check(char *msg, int len) {
  //   int i;

  //   for (i = 0; i < len; i++) {
  //     if (isprint(msg[i]) == 0) {
  //       printf("\n***[DEBUG] not printable!: %c***", msg[i]);
  //       return -1; // not printable!
  //     }
  //   }
  return 1; // success
}

int http_req_parser(char *msg, HTTP_req_t *http_req) {
  // takes in pointer to the client msg and a pointer to http struct
  // walks through lines and populates http struct

  printf("\n[DEBUG] http_req_parser reached");

  char *line_ptr;
  char *line_save_ptr;
  char *token;
  char *token_save_ptr;
  int token_len;

  char *inline_ptr;
  char *inline_save_ptr;

  char *field;

  line_ptr = strtok_r(msg, CRLF, &line_save_ptr);
  token = strtok_r(line_ptr, " ", &token_save_ptr);

  // first line
  // parsing method
  if (token != NULL) {
    token_len = strlen(token);
    http_req->req_line.method = calloc(token_len + 1, sizeof(char));
    if (http_req->req_line.method == NULL)
      return -1;
    strncpy(http_req->req_line.method, token, token_len + 1);
  }

  // first line
  // parsing uri
  token = strtok_r(NULL, " ", &token_save_ptr);
  if (token != NULL) {
    token_len = strlen(token);
    http_req->req_line.uri = calloc(token_len + 1, sizeof(char));
    if (http_req->req_line.uri == NULL)
      return -1;
    strncpy(http_req->req_line.uri, token, token_len + 1);
  }


  // first line
  // parsing version
  token = strtok_r(NULL, " ", &token_save_ptr);
  if (token != NULL) {
    token_len = strlen(token);
    http_req->req_line.version = calloc(token_len + 1, sizeof(char));
    if (http_req->req_line.version == NULL)
      return -1;
    strncpy(http_req->req_line.version, token, token_len + 1);
  }


  // loop through headers to find host and user_agent
  line_ptr = strtok_r(NULL, CRLF, &line_save_ptr);
  while (line_ptr != NULL) {
    // walking horizontally through the lines
    // the token is either a key or a value

    inline_ptr = strtok_r(line_ptr," ", &inline_save_ptr);

    while(inline_ptr != NULL) {
      token_len = strlen(inline_ptr); 

      // saving context for the next iteration in field
      if(strncmp(inline_ptr, "Host:",token_len) == 0 ) {
        field = http_req->host;
      } else if (strncmp(inline_ptr, "User-Agent:",token_len) == 0 ) {
        field = http_req->user_agent;
      } else {
        // this is the value
        if(field == http_req->host) {
          // if the field points to the same place as the req->host
          // previous iteration was the key "Host:"
          // so this one is the value of host
          http_req->host = calloc(token_len+1, sizeof(char));
          if (http_req->host == NULL) return -1;
          strncpy(http_req->host, inline_ptr,token_len +1);
        }
        if(field == http_req->user_agent) {
          http_req->user_agent = calloc(token_len+1, sizeof(char));
          if (http_req->user_agent == NULL) return -1;
          strncpy(http_req->user_agent, inline_ptr,token_len +1);
        }
      }
      inline_ptr = strtok_r(NULL," ", &inline_save_ptr);

    }


    line_ptr = strtok_r(NULL, CRLF, &line_save_ptr);

  }
  return 0;
}

void clean(char **buf) {
  free(*buf);
  *buf = NULL;
}