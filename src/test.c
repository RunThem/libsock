#include "libsock.h"

#include <stdio.h>

void test_func(char* msg, sock_conf_t conf) {
  ret_t ret = 0;

  ret = sock_open(&conf);
  fprintf(stderr, "[test] flag(%s) test:\n    ret is %d, fd is %d\n\n", msg, ret, conf.fd);
}

int main(int argc, const char* argv[]) {
  /*
   * IPv4
   * */
  test_func("ipv4, tcp, server",
            (sock_conf_t){
                .type   = SOCK_TYPE_INET4_TCP,
                .host   = "127.0.0.1",
                .port   = 8080,
                .listen = 1,
            });

  test_func("ipv4, tcp, client",
            (sock_conf_t){
                .type = SOCK_TYPE_INET4_TCP,
                .host = "127.0.0.1",
                .port = 8080,
            });

  test_func("ipv4, tcp, server, nonblock",
            (sock_conf_t){
                .type     = SOCK_TYPE_INET4_TCP,
                .host     = "127.0.0.1",
                .port     = 8081,
                .listen   = 1,
                .nonblock = true,
            });

  test_func("ipv4, tcp, client, nonblock",
            (sock_conf_t){
                .type     = SOCK_TYPE_INET4_TCP,
                .host     = "127.0.0.1",
                .port     = 8081,
                .nonblock = true,
            });

  test_func("ipv4, udp, server",
            (sock_conf_t){
                .type   = SOCK_TYPE_INET4_UDP,
                .host   = "127.0.0.1",
                .port   = 8082,
                .listen = 1,
            });

  test_func("ipv4, udp, client",
            (sock_conf_t){
                .type = SOCK_TYPE_INET4_UDP,
                .host = "127.0.0.1",
                .port = 8082,
            });

  test_func("ipv4, udp, server, nonblock",
            (sock_conf_t){
                .type     = SOCK_TYPE_INET4_UDP,
                .host     = "127.0.0.1",
                .port     = 8083,
                .listen   = 1,
                .nonblock = true,
            });

  test_func("ipv4, udp, client, nonblock",
            (sock_conf_t){
                .type     = SOCK_TYPE_INET4_UDP,
                .host     = "127.0.0.1",
                .port     = 8083,
                .nonblock = true,
            });

  /*
   * IPv6
   * */
  test_func("ipv6, tcp, server",
            (sock_conf_t){
                .type   = SOCK_TYPE_INET6_TCP,
                .host   = "::1",
                .port   = 8090,
                .listen = 1,
            });

  test_func("ipv6, tcp, client",
            (sock_conf_t){
                .type = SOCK_TYPE_INET6_TCP,
                .host = "::1",
                .port = 8090,
            });

  test_func("ipv6, tcp, server, nonblock",
            (sock_conf_t){
                .type   = SOCK_TYPE_INET6_TCP,
                .host   = "::1",
                .port   = 8091,
                .listen = 1,
            });

  test_func("ipv6, tcp, client, nonblock",
            (sock_conf_t){
                .type = SOCK_TYPE_INET6_TCP,
                .host = "::1",
                .port = 8091,
            });

  test_func("ipv6, udp, server",
            (sock_conf_t){
                .type   = SOCK_TYPE_INET6_UDP,
                .host   = "::1",
                .port   = 8092,
                .listen = 1,
            });

  test_func("ipv6, udp, client",
            (sock_conf_t){
                .type = SOCK_TYPE_INET6_UDP,
                .host = "::1",
                .port = 8092,
            });

  test_func("ipv6, udp, server, nonblock",
            (sock_conf_t){
                .type   = SOCK_TYPE_INET6_UDP,
                .host   = "::1",
                .port   = 8093,
                .listen = 1,
            });

  test_func("ipv6, udp, client, nonblock",
            (sock_conf_t){
                .type = SOCK_TYPE_INET6_UDP,
                .host = "::1",
                .port = 8093,
            });

  /*
   * LOCAL
   * */
  test_func("local, tcp, server",
            (sock_conf_t){
                .type   = SOCK_TYPE_LOCAL_TCP,
                .host   = "local_tcp_server.sock",
                .listen = 1,
            });

  test_func("local, tcp, client",
            (sock_conf_t){
                .type = SOCK_TYPE_LOCAL_TCP,
                .host = "local_tcp_server.sock",
            });

  test_func("local, tcp, server, nonblock",
            (sock_conf_t){
                .type   = SOCK_TYPE_LOCAL_TCP,
                .host   = "local_tcp_server_nonblock.sock",
                .listen = 1,
            });

  test_func("local, tcp, client, nonblock",
            (sock_conf_t){
                .type = SOCK_TYPE_LOCAL_TCP,
                .host = "local_tcp_server_nonblock.sock",
            });

  test_func("local, udp, server",
            (sock_conf_t){
                .type   = SOCK_TYPE_LOCAL_UDP,
                .host   = "local_udp_server.sock",
                .listen = 1,
            });

  test_func("local, udp, client",
            (sock_conf_t){
                .type = SOCK_TYPE_LOCAL_UDP,
                .host = "local_udp_server.sock",
            });

  test_func("local, udp, server, nonblock",
            (sock_conf_t){
                .type   = SOCK_TYPE_LOCAL_UDP,
                .host   = "local_udp_server_nonblock.sock",
                .listen = 1,
            });

  test_func("local, udp, client, nonblock",
            (sock_conf_t){
                .type = SOCK_TYPE_LOCAL_UDP,
                .host = "local_udp_server_nonblock.sock",
            });

  return 0;
}
