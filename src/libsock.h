#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <sys/un.h>
#include <unistd.h>

typedef struct sock_conf_t sock_conf_t;
struct sock_conf_t {
  /* if it is not zero, it is in server mode, and the value is the maximum number of connection
   * requests. */
  int listen;

#define SOCK_TYPE_INET4_TCP 1
#define SOCK_TYPE_INET6_TCP 2
#define SOCK_TYPE_INET4_UDP 3
#define SOCK_TYPE_INET6_UDP 4
#define SOCK_TYPE_LOCAL_TCP 5
#define SOCK_TYPE_LOCAL_UDP 6
  int type;

  /* server mode: the listening address and port.
   * client mode: the server address and port. */
  char* host;
  in_port_t port;

  /* server mode: not used.
   * client mode: the local address and port. */
  char* lhost;
  in_port_t lport;

  /* setsockopt & fnctl. */
  bool opt_nonblock;
  bool opt_debug;
  bool opt_keepalive;
  bool opt_reuseaddr;
  bool opt_reuseport;
  int opt_sndbuf;
  int opt_rcvbuf;
};

#define SOCK_RET_OK          (0)  /* Ok */
#define SOCK_RET_CHECK       (-1) /* Parameter error */
#define SOCK_RET_SOCKET      (-2) /* socket() call failed */
#define SOCK_RET_BIND        (-3) /* bind() call failed */
#define SOCK_RET_LISTEN      (-4) /* listen() call failed */
#define SOCK_RET_CONNECT     (-5) /* connect() call failed */
#define SOCK_RET_FCNTL       (-6) /* fcntl() call failed */
#define SOCK_RET_SETSOCKOPT  (-7) /* setsockopt() call failed */
#define SOCK_RET_GETADDRINFO (-8) /* getaddrinfo() call failed */

extern int sock_open(sock_conf_t* conf);

#ifdef __cplusplus
} /* extern "C" */
#endif
