#include "server.h"

#include "ps2keyboard.h"

#include <esp_log.h>

#include <lwip/err.h>
#include <lwip/netdb.h>
#include <lwip/sockets.h>
#include <lwip/sys.h>

#define TCP_SERVER_PORT 0xCAFE
#define TCP_SERVER_KEEPALIVE_IDLE 5
#define TCP_SERVER_KEEPALIVE_INTERVAL 5
#define TCP_SERVER_KEEPALIVE_COUNT 3

static void
do_retransmit(const int sock)
{
  char rx_buffer[128];
  int len = 0;

  do {
    len = recv(sock, rx_buffer, sizeof(rx_buffer) - 1, 0);

    // Error occurred
    if (len < 0) {
      ESP_LOGE(TAG, "Error occurred during receiving: errno %d", errno);
    }
    // Connection closed
    else if (len == 0) {
      ESP_LOGW(TAG, "Connection closed");
    }
    // Data received
    else {
      rx_buffer[len] = 0;

      ESP_LOGI(TAG, "Received %d bytes: %s", len, rx_buffer);

      int to_write = len;
      while (to_write > 0) {
        int written = send(sock, rx_buffer + (len - to_write), to_write, 0);
        if (written < 0) {
          ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
          return;
        }
        to_write -= written;
      }
    }
  } while (len > 0);
}

void
tcp_server_run(void* pvParameters)
{
  struct sockaddr_in dest_addr;
  dest_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  dest_addr.sin_family = AF_INET;
  dest_addr.sin_port = htons(TCP_SERVER_PORT);

  // Create socket
  int listen_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
  if (listen_sock < 0) {
    ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
    vTaskDelete(NULL);
    return;
  }
  int opt = 1;
  setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  ESP_LOGI(TAG, "Socket created");

  int err = bind(listen_sock, (struct sockaddr*)&dest_addr, sizeof(dest_addr));
  if (err != 0) {
    ESP_LOGE(TAG, "Socket unable to bind (%d)", errno);
    goto CLEAN_UP;
  }
  ESP_LOGI(TAG, "Socket bound, port %d", TCP_SERVER_PORT);

  err = listen(listen_sock, 1);
  if (err != 0) {
    ESP_LOGE(TAG, "Error occurred during listen (%d)", errno);
    goto CLEAN_UP;
  }

  while (1) {
    ESP_LOGI(TAG, "Socket listening");

    // Accept connection
    struct sockaddr_in source_addr;
    socklen_t addr_len = sizeof(source_addr);
    int sock = accept(listen_sock, (struct sockaddr*)&source_addr, &addr_len);
    if (sock < 0) {
      ESP_LOGE(TAG, "Unable to accept connection: errno %d", errno);
      break;
    }

    // Set socket options
    {
      int keepAlive = 1;
      int keepIdle = TCP_SERVER_KEEPALIVE_IDLE;
      int keepInterval = TCP_SERVER_KEEPALIVE_INTERVAL;
      int keepCount = TCP_SERVER_KEEPALIVE_COUNT;

      setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, &keepAlive, sizeof(int));
      setsockopt(sock, IPPROTO_TCP, TCP_KEEPIDLE, &keepIdle, sizeof(int));
      setsockopt(sock, IPPROTO_TCP, TCP_KEEPINTVL, &keepInterval, sizeof(int));
      setsockopt(sock, IPPROTO_TCP, TCP_KEEPCNT, &keepCount, sizeof(int));
    }

    // Log IP address
    {
      char addr_str[128];
      inet_ntoa_r(((struct sockaddr_in*)&source_addr)->sin_addr,
                  addr_str,
                  sizeof(addr_str) - 1);

      ESP_LOGI(TAG, "Socket accepted ip address: %s", addr_str);
    }

    do_retransmit(sock);

    // Close socket
    shutdown(sock, 0);
    close(sock);
  }

CLEAN_UP:
  close(listen_sock);
  vTaskDelete(NULL);
}