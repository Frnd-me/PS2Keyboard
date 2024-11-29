#include "main.h"

// PS2Keyboard
#include "ps2keyboard.h"
#include "server.h"
#include "wifi.h"

// ESP
#include <esp_event.h>
#include <esp_log.h>
#include <esp_netif.h>
#include <nvs_flash.h>

// FreeRTOS
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

void
app_main(void)
{
  ESP_ERROR_CHECK(nvs_flash_init());
  ESP_ERROR_CHECK(esp_netif_init());

  // Initialize WIFI
  wifi_init_sta();

  // Create TCP server task
  xTaskCreate(tcp_server_run, // Task function
              "tcp_server",   // Task name
              4096,           // Stack depth
              &handle_key,    // Parameters
              5,              // Priority
              NULL            // Task handle
  );
}