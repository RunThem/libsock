#include <libsock.h>
#include <stdio.h>

#define log(fmt, ...) fprintf(stderr, fmt "\n", ##__VA_ARGS__)

int main(int argc, const char* argv[]) {

  switch (*argv[1]) {
    case '1': {
      log("[ipv4, tcp]:");

      int buff    = 0;
      int fd      = 0;
      size_t size = 0;

      sock_conf_t conf = {
          .type          = SOCK_TYPE_INET4_TCP,
          .host          = "127.0.0.1",
          .port          = 8080,
          .lhost         = "127.0.0.1",
          .lport         = 9999,
          .opt_reuseaddr = true,
      };

      fd = sock_open(&conf);
      log("  connect fd is %d", fd);

      size = send(fd, &(int){1}, sizeof(int), 0);
      log("    send %ld, buff[0] = %d", size, 1);

      size = recv(fd, &buff, sizeof(int), 0);
      log("    recv %ld, buff[0] = %d", size, buff);

      close(fd);
    } break;
    case '2': {
      log("[ipv4, udp]:");

      int buff    = 0;
      int fd      = 0;
      size_t size = 0;

      sock_conf_t conf = {
          .type          = SOCK_TYPE_INET4_UDP,
          .host          = "127.0.0.1",
          .port          = 8080,
          .lhost         = "127.0.0.1",
          .lport         = 9999,
          .opt_reuseaddr = true,
      };

      fd = sock_open(&conf);
      log("  connect fd is %d", fd);

      size = send(fd, &(int){1}, sizeof(int), 0);
      log("    send %ld, buff[0] = %d", size, 1);

      size = recv(fd, &buff, sizeof(int), 0);
      log("    recv %ld, buff[0] = %d", size, buff);

      close(fd);
    } break;
    case '3': {
      log("[ipv6, tcp]:");

      int buff    = 0;
      int fd      = 0;
      size_t size = 0;

      sock_conf_t conf = {
          .type          = SOCK_TYPE_INET6_TCP,
          .host          = "::1",
          .port          = 8080,
          .lhost         = "::1",
          .lport         = 9999,
          .opt_reuseaddr = true,
      };

      fd = sock_open(&conf);
      log("  connect fd is %d", fd);

      size = send(fd, &(int){1}, sizeof(int), 0);
      log("    send %ld, buff[0] = %d", size, 1);

      size = recv(fd, &buff, sizeof(int), 0);
      log("    recv %ld, buff[0] = %d", size, buff);

      close(fd);
    } break;
    case '4': {
      log("[ipv6, udp]:");

      int buff    = 0;
      int fd      = 0;
      size_t size = 0;

      sock_conf_t conf = {
          .type          = SOCK_TYPE_INET6_UDP,
          .host          = "::1",
          .port          = 8080,
          .lhost         = "::1",
          .lport         = 9999,
          .opt_reuseaddr = true,
      };

      fd = sock_open(&conf);
      log("  connect fd is %d", fd);

      size = send(fd, &(int){1}, sizeof(int), 0);
      log("    recv %ld, buff[0] = %d", size, 1);

      size = recv(fd, &buff, sizeof(int), 0);
      log("    recv %ld, buff[0] = %d", size, buff);

      close(fd);
    } break;
    case '5': {
      log("[local, tcp]:");

      int buff    = 0;
      int fd      = 0;
      size_t size = 0;

      sock_conf_t conf = {
          .type          = SOCK_TYPE_LOCAL_TCP,
          .host          = "tcp.sock",
          .lhost         = "local-tcp.sock",
          .opt_reuseaddr = true,
      };

      fd = sock_open(&conf);
      log("  connect fd is %d", fd);

      size = send(fd, &(int){1}, sizeof(int), 0);
      log("    recv %ld, buff[0] = %d", size, 1);

      size = recv(fd, &buff, sizeof(int), 0);
      log("    recv %ld, buff[0] = %d", size, buff);

      close(fd);
    } break;
    case '6': {
      log("[local, udp]:");

      int buff    = 0;
      int fd      = 0;
      size_t size = 0;

      sock_conf_t conf = {
          .type          = SOCK_TYPE_LOCAL_UDP,
          .host          = "udp.sock",
          .lhost         = "local-udp.sock",
          .opt_reuseaddr = true,
      };

      fd = sock_open(&conf);
      log("  connect fd is %d", fd);

      size = send(fd, &(int){1}, sizeof(int), 0);
      log("    send %ld, buff[0] = %d", size, 1);

      size = recv(fd, &buff, sizeof(int), 0);
      log("    recv %ld, buff[0] = %d", size, buff);

      close(fd);
    } break;

    default: break;
  }

  return 0;
}
