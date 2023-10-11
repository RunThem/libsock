#include <libsock.h>
#include <stdio.h>

#define log(fmt, ...) fprintf(stderr, fmt "\n", ##__VA_ARGS__)

int main(int argc, const char* argv[]) {

  switch (*argv[1]) {
    case '1': {
      log("[ipv4, tcp]:");

      int buff                        = 0;
      int fd                          = 0;
      int cfd                         = 0;
      size_t size                     = 0;
      struct sockaddr_in addr         = {0};
      socklen_t addr_len              = sizeof(addr);
      char addr_buff[INET_ADDRSTRLEN] = {0};

      sock_conf_t conf = {
          .type          = SOCK_TYPE_INET4_TCP,
          .host          = "127.0.0.1",
          .port          = 8080,
          .listen        = 1,
          .opt_reuseaddr = true,
      };

      fd = sock_open(&conf);
      log("  listen fd is %d", fd);

      cfd = accept(fd, (struct sockaddr*)&addr, &addr_len);

      inet_ntop(AF_INET, &addr.sin_addr, addr_buff, INET_ADDRSTRLEN);
      log("    accept fd is %d, {%s:%d}", cfd, addr_buff, ntohs(addr.sin_port));

      size = recv(cfd, &buff, sizeof(int), 0);
      log("    recv %ld, buff[0] = %d", size, buff);

      size = send(cfd, &(int){1}, sizeof(int), 0);
      log("    send %ld, buff[0] = %d", size, 1);

      close(fd);
      close(cfd);
    } break;
    case '2': {
      log("[ipv4, udp]:");

      int buf                         = 0;
      int fd                          = 0;
      size_t size                     = 0;
      struct sockaddr_in addr         = {0};
      socklen_t addr_len              = sizeof(addr);
      char addr_buff[INET_ADDRSTRLEN] = {0};

      sock_conf_t conf = {
          .type          = SOCK_TYPE_INET4_UDP,
          .host          = "127.0.0.1",
          .port          = 8080,
          .listen        = 1,
          .opt_reuseaddr = true,
      };

      fd = sock_open(&conf);
      log("  listen fd is %d", fd);

      size = recvfrom(fd, &buf, sizeof(int), 0, &addr, &addr_len);

      inet_ntop(AF_INET, &addr.sin_addr, addr_buff, INET_ADDRSTRLEN);
      log("    recv %ld, buff[0] = %d, {%s:%d}", size, buf, addr_buff, ntohs(addr.sin_port));

      size = sendto(fd, &(int){1}, sizeof(int), 0, &addr, addr_len);
      log("    send %ld, buff[0] = %d", size, 1);

      close(fd);
    } break;
    case '3': {
      log("[ipv6, tcp]:");

      int buf                          = 0;
      int fd                           = 0;
      int cfd                          = 0;
      size_t size                      = 0;
      struct sockaddr_in6 addr         = {0};
      socklen_t addr_len               = sizeof(addr);
      char addr_buff[INET6_ADDRSTRLEN] = {0};

      sock_conf_t conf = {
          .type          = SOCK_TYPE_INET6_TCP,
          .host          = "::1",
          .port          = 8080,
          .listen        = 1,
          .opt_reuseaddr = true,
      };

      fd = sock_open(&conf);
      log("  listen fd is %d", fd);

      cfd = accept(fd, (struct sockaddr*)&addr, &addr_len);
      inet_ntop(AF_INET6, &addr.sin6_addr, addr_buff, INET6_ADDRSTRLEN);
      log("    accept fd is %d, {%s:%d}", cfd, addr_buff, ntohs(addr.sin6_port));

      size = recv(cfd, &buf, sizeof(int), 0);
      log("    recv %ld, buff[0] = %d", size, buf);

      size = send(cfd, &(int){1}, sizeof(int), 0);
      log("    send %ld, buff[0] = %d", size, 1);

      close(fd);
      close(cfd);
    } break;
    case '4': {
      log("[ipv6, udp]:");

      int buf                          = 0;
      int fd                           = 0;
      size_t size                      = 0;
      struct sockaddr_in6 addr         = {0};
      socklen_t addr_len               = sizeof(addr);
      char addr_buff[INET6_ADDRSTRLEN] = {0};

      sock_conf_t conf = {
          .type          = SOCK_TYPE_INET6_UDP,
          .host          = "::1",
          .port          = 8080,
          .listen        = 1,
          .opt_reuseaddr = true,
      };

      fd = sock_open(&conf);
      log("  listen fd is %d", fd);

      size = recvfrom(fd, &buf, sizeof(int), 0, &addr, &addr_len);

      inet_ntop(AF_INET6, &addr.sin6_addr, addr_buff, INET6_ADDRSTRLEN);
      log("    recv %ld, buff[0] = %d, {%s:%d}", size, buf, addr_buff, ntohs(addr.sin6_port));

      sendto(fd, &(int){1}, sizeof(int), 0, &addr, addr_len);
      log("    send %ld, buff[0] = %d", size, 1);

      close(fd);
    } break;
    case '5': {
      log("[local, tcp]:");

      int buf                 = 0;
      int fd                  = 0;
      int cfd                 = 0;
      size_t size             = 0;
      struct sockaddr_un addr = {0};
      socklen_t addr_len      = sizeof(addr);

      sock_conf_t conf = {
          .type          = SOCK_TYPE_LOCAL_TCP,
          .host          = "tcp.sock",
          .listen        = 1,
          .opt_reuseaddr = true,
      };

      fd = sock_open(&conf);
      log("  listen fd is %d", fd);

      cfd = accept(fd, (struct sockaddr*)&addr, &addr_len);
      log("    accept fd is %d, {%s}", fd, addr.sun_path);

      size = recv(cfd, &buf, sizeof(int), 0);
      log("    recv %ld, buff[0] = %d", size, buf);

      size = send(cfd, &(int){1}, sizeof(int), 0);
      log("    recv %ld, buff[0] = %d", size, 1);

      close(fd);
      close(cfd);
    } break;
    case '6': {
      log("[local, udp]:");

      int buf                 = 0;
      int fd                  = 0;
      size_t size             = 0;
      struct sockaddr_un addr = {0};
      socklen_t addr_len      = sizeof(addr);

      sock_conf_t conf = {
          .type          = SOCK_TYPE_LOCAL_UDP,
          .host          = "udp.sock",
          .listen        = 1,
          .opt_reuseaddr = true,
      };

      fd = sock_open(&conf);
      log("  listen fd is %d", fd);

      size = recvfrom(fd, &buf, sizeof(int), 0, (struct sockaddr*)&addr, &addr_len);
      log("    recv %ld, buff[0] = %d, {%s}", size, buf, addr.sun_path);

      size = sendto(fd, &(int){1}, sizeof(int), 0, (struct sockaddr*)&addr, addr_len);
      log("    recv %ld, buff[0] = %d", size, 1);

      close(fd);
    } break;

    default: {
    } break;
  }

  return 0;
}
