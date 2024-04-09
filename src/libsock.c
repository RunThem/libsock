#include "libsock.h"

/* libs */
#include <fcntl.h>
#include <netdb.h>
#include <stdint.h>
#include <stdio.h>

#undef u_if
#define u_if(expr, _ret)                                                                           \
  do {                                                                                             \
    if (expr) {                                                                                    \
      ret = _ret;                                                                                  \
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
  ret_t ret = SOCK_RET_OK;

  /* check port */
  u_if(conf->port > UINT16_MAX, SOCK_RET_CHECK);
  info->port = conf->port;

  /* TODO: check host */
  info->host      = conf->host;
  info->host_size = strlen(conf->host);

  u_if(conf->type < SOCK_TYPE_INET4_TCP || conf->type > SOCK_TYPE_LOCAL_UDP, SOCK_RET_CHECK);

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
  return ret;
}

static int sock_set_opt(sock_conf_t* conf, int fd) {
  ret_t ret       = 0;
  sock_opt_t* opt = NULL;

  for (size_t i = 0; i < SOCK_MAX_OPTS; i++) {
    opt = &conf->opts[i];
    if (opt->opt_id == 0) {
      break;
    }

    ret = setsockopt(fd, SOL_SOCKET, opt->opt_id, opt->value, opt->value_len);
    u_if(ret == -1, SOCK_RET_SETSOCKOPT);
  }

  return SOCK_RET_OK;

err:
  return ret;
}

static int sock_set_nonblock(sock_conf_t* conf) {
  ret_t ret = 0;
  int flags = 0;

  ret = fcntl(conf->fd, F_GETFL, 0);
  u_if(ret == -1, SOCK_RET_FCNTL);

  ret = fcntl(conf->fd, F_SETFL, flags | O_NONBLOCK);
  u_if(ret == -1, SOCK_RET_FCNTL);

  return SOCK_RET_OK;

err:
  return ret;
}

static int sock_open_inet(sock_conf_t* conf, sock_info_t* info) {
  ret_t ret                 = 0;
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

  ret = getaddrinfo(conf->host, port_buf, &hints, &res);
  u_if(ret != 0, SOCK_RET_GETADDRINFO);

  for (ai = res; ai != NULL; ai = ai->ai_next) {
    fd = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
    if (fd == -1) {
      ret = SOCK_RET_SOCKET;
      continue;
    }

    ret = sock_set_opt(conf, fd);
    u_if(ret != SOCK_RET_OK, ret);

    /* server */
    if (conf->listen > 0) {
      ret = bind(fd, res->ai_addr, res->ai_addrlen);
      u_if(ret == -1, SOCK_RET_BIND);

      /* TCP server */
      if (info->type == SOCK_STREAM) {
        ret = listen(fd, conf->listen);
        u_if(ret == -1, SOCK_RET_LISTEN);
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

          ret = bind(fd, (struct sockaddr*)&addr4, sizeof(struct sockaddr_in));
        } else if (info->family == AF_INET6) {
          addr6.sin6_family = AF_INET6;
          addr6.sin6_port   = htons(conf->lport);

          if (conf->lhost == NULL) {
            addr6.sin6_addr = in6addr_any;
          } else {
            inet_pton(AF_INET6, conf->lhost, &addr6.sin6_addr);
          }

          ret = bind(fd, (struct sockaddr*)&addr6, sizeof(struct sockaddr_in6));
        }

        u_if(ret == -1, SOCK_RET_BIND);
      }

      if (info->type == SOCK_DGRAM) {
        break;
      }

      ret = connect(fd, ai->ai_addr, ai->ai_addrlen);
      if (ret == 0) {
        break;
      }

      close(fd);
      fd  = -1;
      ret = SOCK_RET_CONNECT;
    }
  }

  u_if(fd == -1, ret);

  freeaddrinfo(res);
  conf->fd = fd;

  return 0;

err:
  if (fd != -1) {
    close(fd);
  }

  freeaddrinfo(res);

  return ret;
}

static int sock_open_unix(sock_conf_t* conf, sock_info_t* info) {
  ret_t ret               = 0;
  int fd                  = 0;
  struct sockaddr_un addr = {0};
  struct sockaddr_un host = {0};
  socklen_t addr_len      = sizeof(addr);

  fd = socket(info->family, info->type, info->protocol);
  u_if(fd == -1, SOCK_RET_SOCKET);

  ret = sock_set_opt(conf, fd);
  u_if(ret != SOCK_RET_OK, ret);

  addr.sun_family = AF_LOCAL;
  strncpy(addr.sun_path, info->host, info->host_size);

  /* server */
  if (conf->listen > 0) {
    ret = bind(fd, (struct sockaddr*)&addr, addr_len);
    u_if(ret == -1, SOCK_RET_BIND);

    if (info->type == SOCK_STREAM) {
      ret = listen(fd, conf->listen);
      u_if(ret == -1, SOCK_RET_LISTEN);
    }
  } else {
    if (conf->lhost != NULL) {
      host.sun_family = AF_LOCAL;
      strncpy(host.sun_path, conf->host, strlen(conf->host));

      ret = bind(fd, (struct sockaddr*)&host, sizeof(struct sockaddr_un));
      u_if(ret == -1, SOCK_RET_BIND);
    }

    ret = connect(fd, (struct sockaddr*)&addr, addr_len);
    u_if(ret == -1, SOCK_RET_CONNECT);
  }

  conf->fd = fd;

  return 0;

err:
  if (fd != -1) {
    close(fd);
  }

  return ret;
}

int sock_open(sock_conf_t* conf) {
  ret_t ret        = 0;
  sock_info_t info = {0};

  u_if(conf == NULL, SOCK_RET_CHECK);

  ret = sock_check(conf, &info);
  u_if(ret != SOCK_RET_OK, ret);

  conf->fd = -1;
  switch (conf->type) {
    case SOCK_TYPE_LOCAL_TCP:
    case SOCK_TYPE_LOCAL_UDP:
      ret = sock_open_unix(conf, &info);
      break;
    case SOCK_TYPE_INET4_TCP:
    case SOCK_TYPE_INET4_UDP:
    case SOCK_TYPE_INET6_TCP:
    case SOCK_TYPE_INET6_UDP:
      ret = sock_open_inet(conf, &info);
      break;

    default:
      break;
  }

  u_if(ret != SOCK_RET_OK, ret);

  if (conf->nonblock) {
    ret = sock_set_nonblock(conf);
    u_if(ret != SOCK_RET_OK, ret);
  }

  return SOCK_RET_OK;

err:
  return ret;
}

int sock_close(sock_conf_t* conf) {
  ret_t ret = 0;

  u_if(conf == NULL, SOCK_RET_CHECK);
  u_if(conf->fd == -1, SOCK_RET_CHECK);

  close(conf->fd);

  if (conf->type == SOCK_TYPE_LOCAL_UDP || conf->type == SOCK_TYPE_LOCAL_TCP) {
    if (conf->host != NULL) {
      unlink(conf->host);
    }

    if (conf->lhost != NULL) {
      unlink(conf->host);
    }
  }

  return SOCK_RET_OK;

err:
  return ret;
}
