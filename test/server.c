#include <libsock.h>
#include <stdio.h>

#define log(fmt, ...) fprintf(stderr, fmt "\n", ##__VA_ARGS__)

int main(int argc, const char* argv[]) {

  switch (*argv[1]) {
    case '1': {
      ret_t ret               = 0;
      int buf                 = 0;
      int fd                  = 0;
      struct sockaddr_in addr = {0};
      socklen_t len           = sizeof(addr);

      sock_conf_t conf = {
          .type   = SOCK_TYPE_INET4_TCP,
          .host   = "127.0.0.1",
          .port   = 8080,
          .listen = 1,
      };

      log("[ipv4, tcp]:");

      /* create socket */
      ret = sock_open(&conf);
      log("  server ret is %d, fd is %d", ret, conf.fd);

      fd = accept(conf.fd, (struct sockaddr*)&addr, &len);
      log("    accept fd is %d", fd);

      /* write & read */
      recv(fd, &buf, sizeof(int), 0);
      log("    client write %s, buf[0] = %d", buf == 1 ? "ok" : "err", buf);

      send(fd, &(int){1}, sizeof(int), 0);

      close(fd);
      close(conf.fd);
    } break;
    case '2': {
      ret_t ret               = 0;
      int buf                 = 0;
      struct sockaddr_in addr = {0};
      socklen_t len           = sizeof(addr);

      sock_conf_t conf = {
          .type   = SOCK_TYPE_INET4_UDP,
          .host   = "127.0.0.1",
          .port   = 8081,
          .listen = 1,
      };

      log("[ipv4, udp]:");

      /* create socket */
      ret = sock_open(&conf);
      log("  server ret is %d, fd is %d", ret, conf.fd);

      /* write & read */
      recvfrom(conf.fd, &buf, sizeof(int), 0, &addr, &len);
      log("    client write %s, buf[0] = %d", buf == 1 ? "ok" : "err", buf);

      sendto(conf.fd, &(int){1}, sizeof(int), 0, &addr, len);

      close(conf.fd);
    } break;
    case '3': {
      ret_t ret                = 0;
      int buf                  = 0;
      int fd                   = 0;
      struct sockaddr_in6 addr = {0};
      socklen_t len            = sizeof(addr);

      sock_conf_t conf = {
          .type   = SOCK_TYPE_INET6_TCP,
          .host   = "::1",
          .port   = 8082,
          .listen = 1,
      };

      log("[ipv6, tcp]:");

      /* create socket */
      ret = sock_open(&conf);
      log("  server ret is %d, fd is %d", ret, conf.fd);

      fd = accept(conf.fd, (struct sockaddr*)&addr, &len);
      log("    accept fd is %d", fd);

      /* write & read */
      recv(fd, &buf, sizeof(int), 0);
      log("    client write %s, buf[0] = %d", buf == 1 ? "ok" : "err", buf);

      send(fd, &(int){1}, sizeof(int), 0);

      close(fd);
      close(conf.fd);
    } break;
    case '4': {
      ret_t ret                = 0;
      int buf                  = 0;
      struct sockaddr_in6 addr = {0};
      socklen_t len            = sizeof(addr);

      sock_conf_t conf = {
          .type   = SOCK_TYPE_INET6_UDP,
          .host   = "::1",
          .port   = 8084,
          .listen = 1,
      };

      log("[ipv6, udp]:");

      /* create socket */
      ret = sock_open(&conf);
      log("  server ret is %d, fd is %d", ret, conf.fd);

      /* write & read */
      recvfrom(conf.fd, &buf, sizeof(int), 0, &addr, &len);
      log("    client write %s, buf[0] = %d", buf == 1 ? "ok" : "err", buf);

      sendto(conf.fd, &(int){1}, sizeof(int), 0, &addr, len);

      close(conf.fd);
    } break;
    case '5': {
      ret_t ret               = 0;
      int buf                 = 0;
      int fd                  = 0;
      struct sockaddr_un addr = {0};
      socklen_t len           = sizeof(addr);

      sock_conf_t conf = {
          .type   = SOCK_TYPE_LOCAL_TCP,
          .host   = "tcp.sock",
          .listen = 1,
      };

      log("[local, tcp]:");

      /* create socket */
      ret = sock_open(&conf);
      log("  server ret is %d, fd is %d", ret, conf.fd);

      fd = accept(conf.fd, (struct sockaddr*)&addr, &len);
      log("    accept fd is %d", fd);

      /* write & read */
      recv(fd, &buf, sizeof(int), 0);
      log("    client write %s, buf[0] = %d", buf == 1 ? "ok" : "err", buf);

      send(fd, &(int){1}, sizeof(int), 0);

      close(fd);
      close(conf.fd);
    } break;
    case '6': {
      ret_t ret = 0;
      int buf   = 0;

      sock_conf_t conf = {
          .type   = SOCK_TYPE_LOCAL_UDP,
          .host   = "udp.sock",
          .listen = 1,
      };

      log("[local, udp]:");

      /* create socket */
      ret = sock_open(&conf);
      log("  server ret is %d, fd is %d", ret, conf.fd);

      /* write & read */
      recv(conf.fd, &buf, sizeof(int), 0);
      log("    client write %s, buf[0] = %d", buf == 1 ? "ok" : "err", buf);

      /*
       * XXX: send() ???
       * */

      close(conf.fd);
      unlink(conf.host);
    } break;
    default:
      break;
  }

  return 0;
}
