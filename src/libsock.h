#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCK_MAX_OPTS 4

typedef int ret_t;

typedef struct sock_opt_t sock_opt_t;
struct sock_opt_t {
  int opt_id;
  void* value;
  socklen_t value_len;
};

typedef struct sock_conf_t sock_conf_t;
struct sock_conf_t {
  int fd;

#define SOCK_TYPE_INET4_TCP 1
#define SOCK_TYPE_INET6_TCP 2
#define SOCK_TYPE_INET4_UDP 3
#define SOCK_TYPE_INET6_UDP 4
#define SOCK_TYPE_LOCAL_TCP 5
#define SOCK_TYPE_LOCAL_UDP 6
  int type;

  char* host;
  in_port_t port;
  int listen;
  bool nonblock;

  sock_opt_t opts[SOCK_MAX_OPTS];
};

#define SOCK_RET_OK          0 /* Ok */
#define SOCK_RET_NULL        1 /* Pointer is NULL */
#define SOCK_RET_CHECK       2 /* Paramerter error */
#define SOCK_RET_SOCKET      3 /* socket() call failed */
#define SOCK_RET_BIND        4 /* bind() call failed */
#define SOCK_RET_LISTEN      5 /* listen() call failed */
#define SOCK_RET_CONNECT     6 /* connect() call failed */
#define SOCK_RET_FCNTL       7 /* fcntl() call failed */
#define SOCK_RET_SETSOCKOPT  8 /* setsockopt() call failed */
#define SOCK_RET_GETADDRINFO 9 /* getaddrinfo() call failed */

int sock_open(sock_conf_t* conf);

#ifdef __cplusplus
} /* extern "C" */
#endif
