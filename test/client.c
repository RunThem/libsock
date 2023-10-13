#include <libsock.h>
#include <stdio.h>

#define log(fmt, ...) fprintf(stderr, fmt "\n", ##__VA_ARGS__)

int main(int argc, const char* argv[]) {

  switch (*argv[1]) {
    case '1': {
      ret_t ret = 0;
      int buf   = 0;

      sock_conf_t conf = {
          .type = SOCK_TYPE_INET4_TCP,
          .host = "127.0.0.1",
          .port = 8080,
      };

      /* create socket */
      ret = sock_open(&conf);
      log("  client ret is %d, fd is %d", ret, conf.fd);

      /* write & read */
      send(conf.fd, &(int){1}, sizeof(int), 0);

      recv(conf.fd, &buf, sizeof(int), 0);
      log("    server write %s, buf[0] = %d", buf == 1 ? "ok" : "err", buf);

      close(conf.fd);
    } break;
    case '2': {
      ret_t ret               = 0;
      int buf                 = 0;
      struct sockaddr_in addr = {0};
      socklen_t len           = sizeof(addr);

      sock_conf_t conf = {
          .type = SOCK_TYPE_INET4_UDP,
          .host = "127.0.0.1",
          .port = 8081,
      };

      /* create socket */
      ret = sock_open(&conf);
      log("  client ret is %d, fd is %d", ret, conf.fd);

      addr.sin_family = AF_INET;
      addr.sin_port   = htons(conf.port);

      inet_pton(AF_INET, conf.host, &addr.sin_addr);

      /* write & read */
      sendto(conf.fd, &(int){1}, sizeof(int), 0, &addr, len);

      recvfrom(conf.fd, &buf, sizeof(int), 0, &addr, &len);
      log("    server write %s, buf[0] = %d", buf == 1 ? "ok" : "err", buf);

      close(conf.fd);
    } break;
    case '3': {
      ret_t ret = 0;
      int buf   = 0;

      sock_conf_t conf = {
          .type = SOCK_TYPE_INET6_TCP,
          .host = "::1",
          .port = 8082,
      };

      log("[ipv6, tcp]:");

      /* create socket */
      ret = sock_open(&conf);
      log("  client ret is %d, fd is %d", ret, conf.fd);

      /* write & read */
      send(conf.fd, &(int){1}, sizeof(int), 0);

      recv(conf.fd, &buf, sizeof(int), 0);
      log("    server write %s, buf[0] = %d", buf == 1 ? "ok" : "err", buf);

      close(conf.fd);
    } break;
    case '4': {
      ret_t ret                = 0;
      int buf                  = 0;
      struct sockaddr_in6 addr = {0};
      socklen_t len            = sizeof(addr);

      sock_conf_t conf = {
          .type = SOCK_TYPE_INET6_UDP,
          .host = "::1",
          .port = 8084,
      };

      /* create socket */
      ret = sock_open(&conf);
      log("  client ret is %d, fd is %d", ret, conf.fd);

      addr.sin6_family = AF_INET6;
      addr.sin6_port   = htons(conf.port);

      inet_pton(AF_INET6, conf.host, &addr.sin6_addr);

      /* write & read */
      sendto(conf.fd, &(int){1}, sizeof(int), 0, &addr, len);

      recvfrom(conf.fd, &buf, sizeof(int), 0, &addr, &len);
      log("    server write %s, buf[0] = %d", buf == 1 ? "ok" : "err", buf);

      close(conf.fd);
    } break;
    case '5': {
      ret_t ret = 0;
      int buf   = 0;

      sock_conf_t conf = {
          .type = SOCK_TYPE_LOCAL_TCP,
          .host = "tcp.sock",
      };

      log("[local, tcp]:");

      /* create socket */
      ret = sock_open(&conf);
      log("  client ret is %d, fd is %d", ret, conf.fd);

      /* write & read */
      send(conf.fd, &(int){1}, sizeof(int), 0);

      recv(conf.fd, &buf, sizeof(int), 0);
      log("    server write %s, buf[0] = %d", buf == 1 ? "ok" : "err", buf);

      close(conf.fd);
    } break;
    case '6': {
      ret_t ret = 0;

      sock_conf_t conf = {
          .type = SOCK_TYPE_LOCAL_UDP,
          .host = "udp.sock",
      };

      log("[local, udp]:");

      /* create socket */
      ret = sock_open(&conf);
      log("  client ret is %d, fd is %d", ret, conf.fd);

      /* write & read */
      send(conf.fd, &(int){1}, sizeof(int), 0);

      /*
       * XXX: recv() ???
       * */

      close(conf.fd);
      unlink(conf.host);
    } break;
    default:
      break;
  }

  return 0;
}
