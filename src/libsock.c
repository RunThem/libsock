#include "libsock.h"

#include <fcntl.h>
#include <netdb.h>
#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>

#define check(expr, ...)                                                                           \
  do {                                                                                             \
    if (expr) {                                                                                    \
      fprintf(stderr,                                                                              \
              "[%s:%d]: [%3d, %s] %s\n",                                                           \
              __FILE__,                                                                            \
              __LINE__,                                                                            \
              errno,                                                                               \
              strerror(errno),                                                                     \
              #expr);                                                                              \
      __VA_ARGS__;                                                                                 \
      goto err;                                                                                    \
    }                                                                                              \
  } while (0)

typedef struct sock_info_t sock_info_t;
struct sock_info_t {
  int family;
  int type;
  int protocol;

  char* host;
  size_t host_size;

  in_port_t port;
};

static int sock_check(sock_conf_t* conf, sock_info_t* info) {
  int error = 0;

  /* check port */
  check(conf->port > UINT16_MAX, error = SOCK_RET_CHECK);
  info->port = conf->port;

  /* TODO: check host */
  info->host      = conf->host;
  info->host_size = strlen(conf->host);

  check(conf->type < SOCK_TYPE_INET4_TCP || conf->type > SOCK_TYPE_LOCAL_UDP,
        error = SOCK_RET_CHECK);

#define set(_family, _type, _protocol)                                                             \
  do {                                                                                             \
    info->family   = _family;                                                                      \
    info->type     = _type;                                                                        \
    info->protocol = _protocol;                                                                    \
  } while (0)

  if (conf->type == SOCK_TYPE_INET4_TCP) {
    set(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  } else if (conf->type == SOCK_TYPE_INET4_UDP) {
    set(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  } else if (conf->type == SOCK_TYPE_INET6_TCP) {
    set(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
  } else if (conf->type == SOCK_TYPE_INET6_UDP) {
    set(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
  } else if (conf->type == SOCK_TYPE_LOCAL_TCP) {
    set(AF_LOCAL, SOCK_STREAM, 0);
  } else if (conf->type == SOCK_TYPE_LOCAL_UDP) {
    set(AF_LOCAL, SOCK_DGRAM, 0);
  }
#undef set

  return SOCK_RET_OK;

err:
  return error;
}

static int sock_set_opt(int fd, sock_conf_t* conf) {
  int error  = 0;
  int result = 0;

  if (conf->opt_nonblock) {
    result = fcntl(fd, F_GETFL, 0);
    check(result == -1, error = SOCK_RET_FCNTL);

    result = fcntl(fd, F_SETFL, result | O_NONBLOCK);
    check(result == -1, error = SOCK_RET_FCNTL);
  }

  if (conf->opt_debug) {
    result = setsockopt(fd, SOL_SOCKET, SO_DEBUG, &(int){1}, sizeof(int));
    check(result == -1, error = SOCK_RET_SETSOCKOPT);
  }

  if (conf->opt_reuseaddr) {
    result = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));
    check(result == -1, error = SOCK_RET_SETSOCKOPT);
  }

  if (conf->opt_reuseport) {
    result = setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &(int){1}, sizeof(int));
    check(result == -1, error = SOCK_RET_SETSOCKOPT);
  }

  if (conf->opt_keepalive) {
    result = setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &(int){1}, sizeof(int));
    check(result == -1, error = SOCK_RET_SETSOCKOPT);
  }

  if (conf->opt_sndbuf) {
    result = setsockopt(fd, SOL_SOCKET, SO_SNDBUF, &conf->opt_sndbuf, sizeof(int));
    check(result == -1, error = SOCK_RET_SETSOCKOPT);
  }

  if (conf->opt_rcvbuf) {
    result = setsockopt(fd, SOL_SOCKET, SO_SNDBUF, &conf->opt_rcvbuf, sizeof(int));
    check(result == -1, error = SOCK_RET_SETSOCKOPT);
  }

  return SOCK_RET_OK;

err:
  return error;
}

static int sock_open_inet(sock_conf_t* conf, sock_info_t* info) {
  int error                 = 0;
  int result                = 0;
  int fd                    = -1;
  char port_buf[6]          = {0};
  struct addrinfo hints     = {0};
  struct addrinfo* res      = NULL;
  struct addrinfo* ai       = NULL;
  struct sockaddr_in addr4  = {0};
  struct sockaddr_in6 addr6 = {0};

  hints.ai_family   = info->family;
  hints.ai_socktype = info->type;
  hints.ai_protocol = info->protocol;

  /* set passive unless UDP client */
  if (!(info->type == SOCK_DGRAM && conf->listen == 0)) {
    hints.ai_flags |= AI_PASSIVE;
  }

  if (info->type != AF_LOCAL) {
    hints.ai_flags |= AI_NUMERICHOST;
  }

  snprintf(port_buf, sizeof(port_buf), "%u", info->port);

  result = getaddrinfo(conf->host, port_buf, &hints, &res);
  check(result != 0, error = SOCK_RET_GETADDRINFO);

  for (ai = res; ai != NULL; ai = ai->ai_next) {
    fd = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
    if (fd != -1) {
      error = 0;
      break;
    }

    error = SOCK_RET_SOCKET;
  }

  check(fd == -1, error = SOCK_RET_SOCKET);

  result = sock_set_opt(fd, conf);
  check(result != SOCK_RET_OK, error = result);

  /* server */
  if (conf->listen > 0) {
    result = bind(fd, res->ai_addr, res->ai_addrlen);
    check(result == -1, error = SOCK_RET_BIND);

    /* TCP server */
    if (info->type == SOCK_STREAM) {
      result = listen(fd, conf->listen);
      check(result == -1, error = SOCK_RET_LISTEN);
    }
  } else { /* client */
    if (conf->lport != 0) {
      if (info->family == AF_INET) {
        addr4.sin_family = AF_INET;
        addr4.sin_port   = htons(conf->lport);

        if (conf->lhost == NULL) {
          addr4.sin_addr.s_addr = htonl(INADDR_ANY);
        } else {
          inet_pton(AF_INET, conf->lhost, &addr4.sin_addr);
        }

        result = bind(fd, (struct sockaddr*)&addr4, sizeof(struct sockaddr_in));
      } else if (info->family == AF_INET6) {
        addr6.sin6_family = AF_INET6;
        addr6.sin6_port   = htons(conf->lport);

        if (conf->lhost == NULL) {
          addr6.sin6_addr = in6addr_any;
        } else {
          inet_pton(AF_INET6, conf->lhost, &addr6.sin6_addr);
        }

        result = bind(fd, (struct sockaddr*)&addr6, sizeof(struct sockaddr_in6));
      }

      check(result == -1, error = SOCK_RET_BIND);
    }

    result = connect(fd, ai->ai_addr, ai->ai_addrlen);
    check(result == -1, error = SOCK_RET_CONNECT);
  }

  freeaddrinfo(res);

  return fd;

err:
  if (fd != -1) {
    close(fd);
  }

  if (res != NULL) {
    freeaddrinfo(res);
  }

  return error;
}

static int sock_open_unix(sock_conf_t* conf, sock_info_t* info) {
  int error               = 0;
  int result              = 0;
  int fd                  = 0;
  struct sockaddr_un addr = {0};
  struct sockaddr_un host = {0};
  socklen_t addr_len      = sizeof(addr);
  struct stat pathstat    = {0};

  fd = socket(info->family, info->type, info->protocol);
  check(fd == -1, error = SOCK_RET_SOCKET);

  result = sock_set_opt(fd, conf);
  check(result != SOCK_RET_OK, error = result);

  addr.sun_family = AF_LOCAL;
  strncpy(addr.sun_path, info->host, info->host_size);

  /* server */
  if (conf->listen > 0) {
    result = stat(conf->host, &pathstat);
    if (result == 0) { /* addr exist. */
      if (S_ISSOCK(pathstat.st_mode)) {
        unlink(conf->host);
      }
    }

    result = bind(fd, (struct sockaddr*)&addr, addr_len);
    check(result == -1, error = SOCK_RET_BIND);

    if (info->type == SOCK_STREAM) {
      result = listen(fd, conf->listen);
      check(result == -1, error = SOCK_RET_LISTEN);
    }
  } else {
    if (conf->lhost != NULL) {
      result = stat(conf->lhost, &pathstat);
      if (result == 0) { /* addr exist. */
        if (S_ISSOCK(pathstat.st_mode)) {
          unlink(conf->lhost);
        }
      }

      host.sun_family = AF_LOCAL;
      strncpy(host.sun_path, conf->lhost, sizeof(host.sun_path) - 1);

      result = bind(fd, (struct sockaddr*)&host, sizeof(struct sockaddr_un));
      check(result == -1, error = SOCK_RET_BIND);
    }

    result = connect(fd, (struct sockaddr*)&addr, addr_len);
    check(result == -1, error = SOCK_RET_CONNECT);
  }

  return fd;

err:
  if (fd != -1) {
    close(fd);
  }

  return error;
}

int sock_open(sock_conf_t* conf) {
  int error        = 0;
  int result       = 0;
  sock_info_t info = {0};

  check(conf == NULL, error = SOCK_RET_CHECK);

  result = sock_check(conf, &info);
  check(result != SOCK_RET_OK, error = result);

  switch (conf->type) {
    case SOCK_TYPE_LOCAL_TCP:
    case SOCK_TYPE_LOCAL_UDP: result = sock_open_unix(conf, &info); break;

    case SOCK_TYPE_INET4_TCP:
    case SOCK_TYPE_INET4_UDP:
    case SOCK_TYPE_INET6_TCP:
    case SOCK_TYPE_INET6_UDP: result = sock_open_inet(conf, &info); break;

    default: break;
  }

  check(result < SOCK_RET_OK, error = result);

  return result;

err:
  return error;
}
