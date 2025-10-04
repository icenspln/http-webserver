#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "../include/client_handler.h"
#include "../include/server.h"

int client_handler(int client_sock) {
  char *client_msg;
  char *server_msg;
  int recv_bytes = 0;
  char *uri;
  FILE *file_fd;
  long file_size;
  char *resource_buff;
  int route;
  HTTP_req_t http_req = {0};
  HTTP_res_t http_res = {0};

  client_msg = (char *)malloc(BUFFER_SIZE);
  server_msg = (char *)calloc(BUFFER_SIZE, sizeof(char));

  while ((recv_bytes = recv(client_sock, client_msg, BUFFER_SIZE, 0)) != 0) {
    if (recv_bytes == -1) {
      printf("[DEBUG] recv failed!!\n");
      clean(&client_msg);
      clean(&server_msg);
      return -1;
    }

    client_msg[recv_bytes] = '\0';

    // parse http req
    if ((http_req_parser(client_msg, &http_req)) == -1) {
      // error parsing not necessarily a error 500
      printf("[DEBUG] [ERROR] http_req_parser failed\n");
      clean(&client_msg);
      clean(&server_msg);
      clean(&http_req.req_line.method);
      clean(&http_req.req_line.uri);
      clean(&http_req.req_line.version);
      clean(&http_req.host);
      clean(&http_req.user_agent);
      return -1;
    }
    // debug
    printf("[INFO] Logging request info:\n");
    printf("[INFO] method: %s\n", http_req.req_line.method);
    printf("[INFO] uri: %s\n", http_req.req_line.uri);
    printf("[INFO] version: %s\n", http_req.req_line.version);
    printf("[INFO] host: %s\n", http_req.host);
    printf("[INFO] user agent: %s\n", http_req.user_agent);

    // routes
    route = get_route(&http_req);

    switch (route) {
    case 0:
      // index route

      file_fd = fopen("./public/index.html", "r");
      if (file_fd == NULL) {
        printf("[DEBUG] fopen failed!\n");
        clean(&client_msg);
        clean(&server_msg);
        clean(&http_req.req_line.method);
        clean(&http_req.req_line.uri);
        clean(&http_req.req_line.version);
        clean(&http_req.host);
        clean(&http_req.user_agent);
        return -1;
      }

      file_size = get_file_size(file_fd);
      if (file_size == -1) {
        printf("[DEBUG] get_file_size failed!\n");
        fclose(file_fd);
        clean(&client_msg);
        clean(&server_msg);
        clean(&http_req.req_line.method);
        clean(&http_req.req_line.uri);
        clean(&http_req.req_line.version);
        clean(&http_req.host);
        clean(&http_req.user_agent);
        return -1;
      }

      resource_buff = calloc(file_size + 1, sizeof(char *));
      if (resource_buff == NULL) {
        printf("[DEBUG] calloc failed!\n");
        fclose(file_fd);
        clean(&client_msg);
        clean(&server_msg);
        clean(&http_req.req_line.method);
        clean(&http_req.req_line.uri);
        clean(&http_req.req_line.version);
        clean(&http_req.host);
        clean(&http_req.user_agent);
        return -1;
      }

      fread(resource_buff, file_size + 1, sizeof(char), file_fd);

      http_res.status_line.status = "200";
      http_res.status_line.reason = "OK";
      http_res.status_line.version = "HTTP/1.1";
      http_res.content_type = "text/html";
      http_res.content_length = file_size;

      server_msg =
          http_res_constructor(resource_buff, strlen(resource_buff), &http_res);

      if (server_msg == NULL) {
        // should not happen, error 500
        printf("[DEBUG] http_res_constructor failed!\n");
        fclose(file_fd);
        clean(&client_msg);
        clean(&server_msg);
        clean(&http_req.req_line.method);
        clean(&http_req.req_line.uri);
        clean(&http_req.req_line.version);
        clean(&http_req.host);
        clean(&http_req.user_agent);
        return -1;
      }

      if (send(client_sock, server_msg, strlen(server_msg), 0) <= 0) {
        // should not happen, error 500
        printf("[DEBUG] send failed!\n");
        fclose(file_fd);
        clean(&client_msg);
        clean(&server_msg);
        clean(&http_req.req_line.method);
        clean(&http_req.req_line.uri);
        clean(&http_req.req_line.version);
        clean(&http_req.host);
        clean(&http_req.user_agent);
        return -1;
      }

      break;
    case -1:
      // 404 not found
      http_res.status_line.status = "404";
      http_res.status_line.reason = "Not Found";
      http_res.status_line.version = "HTTP/1.1";

      server_msg = http_res_constructor(NULL, 0, &http_res);

      if (server_msg == NULL) {
        // should not happen, error 500
        clean(&client_msg);
        clean(&server_msg);
        clean(&http_req.req_line.method);
        clean(&http_req.req_line.uri);
        clean(&http_req.req_line.version);
        clean(&http_req.host);
        clean(&http_req.user_agent);
        return -1;
      }

      if (send(client_sock, server_msg, strlen(server_msg), 0) <= 0) {
        // should not happen, error 500
        clean(&client_msg);
        clean(&server_msg);
        clean(&http_req.req_line.method);
        clean(&http_req.req_line.uri);
        clean(&http_req.req_line.version);
        clean(&http_req.host);
        clean(&http_req.user_agent);
        return -1;
      }

      break;
    }
  }

  return 0; // sucess
}

int http_sanity_check(char *msg, int len) {
  //   int i;

  //   for (i = 0; i < len; i++) {
  //     if (isprint(msg[i]) == 0) {
  //       printf("***[DEBUG] not printable!: %c***\n", msg[i]);
  //       return -1; // not printable!
  //     }
  //   }
  return 1; // success
}

int http_req_parser(char *msg, HTTP_req_t *http_req) {

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

  // parsing method
  if (token != NULL) {
    token_len = strlen(token);
    http_req->req_line.method = calloc(token_len + 1, sizeof(char));
    if (http_req->req_line.method == NULL)
      return -1;
    strncpy(http_req->req_line.method, token, token_len + 1);
  }

  // parsing uri
  token = strtok_r(NULL, " ", &token_save_ptr);
  if (token != NULL) {
    token_len = strlen(token);
    http_req->req_line.uri = calloc(token_len + 1, sizeof(char));
    if (http_req->req_line.uri == NULL)
      return -1;
    strncpy(http_req->req_line.uri, token, token_len + 1);
  }

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

    inline_ptr = strtok_r(line_ptr, " ", &inline_save_ptr);

    while (inline_ptr != NULL) {
      token_len = strlen(inline_ptr);

      // saving context for the next iteration in field
      if (strncmp(inline_ptr, "Host:", token_len) == 0) {
        field = http_req->host;
      } else if (strncmp(inline_ptr, "User-Agent:", token_len) == 0) {
        field = http_req->user_agent;
      } else {
        // this is the value
        if (field == http_req->host) {
          // if the field points to the same place as the req->host
          // previous iteration was the key "Host:"
          // so this one is the value of host
          // and so on..
          http_req->host = calloc(token_len + 1, sizeof(char));
          if (http_req->host == NULL)
            return -1;
          strncpy(http_req->host, inline_ptr, token_len + 1);
        }
        if (field == http_req->user_agent) {
          http_req->user_agent = calloc(token_len + 1, sizeof(char));
          if (http_req->user_agent == NULL)
            return -1;
          strncpy(http_req->user_agent, inline_ptr, token_len + 1);
        }
      }
      inline_ptr = strtok_r(NULL, " ", &inline_save_ptr);
    }

    line_ptr = strtok_r(NULL, CRLF, &line_save_ptr);
  }
  return 0;
}

char *http_res_constructor(char *body, long body_len, HTTP_res_t *http_res) {
  // function that returns the full server_message ready to be sent

  char *http_res_buffer;

  http_res_buffer = calloc(body_len + 100, sizeof(char));
  if (http_res_buffer == NULL) {
    printf("[DEBUG] http_res_constructor failed!\n");
    return NULL;
  }

  // status line
  strncat(http_res_buffer, http_res->status_line.version,
          strlen(http_res->status_line.version) + 1);
  strncat(http_res_buffer, " ", 2);
  strncat(http_res_buffer, http_res->status_line.status,
          strlen(http_res->status_line.status) + 1);
  strncat(http_res_buffer, " ", 2);
  strncat(http_res_buffer, http_res->status_line.reason,
          strlen(http_res->status_line.reason) + 1);
  strncat(http_res_buffer, CRLF, 3);

  if (body != NULL && body_len > 0) {

    // content type & length
    strncat(http_res_buffer, "Content-Type: ", 15);
    strncat(http_res_buffer, http_res->content_type,
            strlen(http_res->content_type) + 1);
    strncat(http_res_buffer, CRLF, 3);

    strncat(http_res_buffer, "Content-Length: ", 17);

    // unsafe, must be changed
    char temp[100];
    sprintf(temp, "%d", http_res->content_length);
    strncat(http_res_buffer, temp, strlen(temp) + 1);

    // body
    strncat(http_res_buffer, CRLF, 3);
    strncat(http_res_buffer, CRLF, 3);
    strncat(http_res_buffer, body, body_len + 1);
  }

  return http_res_buffer;
}

long get_file_size(FILE *fd) {
  long file_size;

  if (fseek(fd, 0, SEEK_END) == -1) {
    fclose(fd);
    return -1;
  }
  file_size = ftell(fd);

  if (file_size == -1) {
    fclose(fd);
    return -1;
  }

  rewind(fd);
  return file_size;
}

void clean(char **buf) {
  if (*buf == NULL) {
    return;
  }
  free(*buf);
  *buf = NULL;
}

int get_route(HTTP_req_t *http_req) {

  int route;
  int uri_length;

  uri_length = strlen(http_req->req_line.uri);

  if (strncmp(http_req->req_line.uri, "/", uri_length) == 0 &&
      strncmp(http_req->req_line.method, "GET", 3) == 0) {
    route = 0;
  } else {
    route = -1; // not found!
  }
  return route;
}