#include "wifi.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "ps2keyboard.h"

// Event group to signal Wi-Fi connection status
static EventGroupHandle_t s_wifi_event_group;

// Event bits to signify connection success or failure
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1

// Counter for Wi-Fi connection retries
static int s_retry_num = 0;

// Wi-Fi event handler function
// This handles various Wi-Fi events, such as starting the connection process,
// retrying on failure, or confirming connection when an IP is received.
static void
event_handler(void* arg,
              esp_event_base_t event_base,
              int32_t event_id,
              void* event_data)
{
  if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
    // When station starts, try connecting to the Access Point (AP)
    esp_wifi_connect();
  } else if (event_base == WIFI_EVENT &&
             event_id == WIFI_EVENT_STA_DISCONNECTED) {
    // Retry connection if disconnected and retry count is below max retries
    if (s_retry_num < 5) {
      esp_wifi_connect();

      ++s_retry_num;
      ESP_LOGI(TAG, "Retrying connection to the AP...");
    } else {
      // Set failure bit if connection retries exceed limit
      xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
    }

    ESP_LOGI(TAG, "Failed to connect to the AP.");
  } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
    // Successfully connected and received IP address
    ip_event_got_ip_t* event = (ip_event_got_ip_t*)event_data;
    ESP_LOGI(TAG, "Got IP: " IPSTR, IP2STR(&event->ip_info.ip));

    // Reset retry counter on success
    s_retry_num = 0;

    xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
  }
}

void
wifi_init_sta()
{
  // Create an event group to manage connection events
  s_wifi_event_group = xEventGroupCreate();

  // Initialize network interface
  ESP_ERROR_CHECK(esp_netif_init());

  // Create default event loop
  ESP_ERROR_CHECK(esp_event_loop_create_default());

  // Create a default station (STA) interface for Wi-Fi
  esp_netif_create_default_wifi_sta();

  // Initialize the Wi-Fi configuration
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));

  // Register Wi-Fi event handlers
  esp_event_handler_instance_t instance_any_id;
  esp_event_handler_instance_t instance_got_ip;
  ESP_ERROR_CHECK(esp_event_handler_instance_register(
    WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL, &instance_any_id));
  ESP_ERROR_CHECK(esp_event_handler_instance_register(
    IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL, &instance_got_ip));

  // Configure Wi-Fi with SSID, password, and authentication mode
  wifi_config_t wifi_config = {
        .sta = {
            .ssid = CONFIG_ESP_WIFI_SSID,
            .password = CONFIG_ESP_WIFI_PASSWORD,
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,  // WPA2 PSK for security
            .sae_pwe_h2e = WPA3_SAE_PWE_BOTH,          // Support WPA3 SAE PWE
            .sae_h2e_identifier = "", // Identifier for WPA3 SAE
        },
    };

  // Set Wi-Fi mode to station
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

  // Apply the configuration to the Wi-Fi interface
  ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));

  // Start the Wi-Fi connection process
  ESP_ERROR_CHECK(esp_wifi_start());

  ESP_LOGI(TAG, "Wi-Fi station initialized.");

  // Wait until connection is established or failed
  EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
                                         WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
                                         pdFALSE,
                                         pdFALSE,
                                         portMAX_DELAY);

  // Check which event occurred (connected or failed)
  if (bits & WIFI_CONNECTED_BIT) {
    ESP_LOGI(TAG,
             "Successfully connected to SSID:%s, password:%s",
             CONFIG_ESP_WIFI_SSID,
             CONFIG_ESP_WIFI_PASSWORD);
  } else if (bits & WIFI_FAIL_BIT) {
    ESP_LOGI(TAG,
             "Failed to connect to SSID:%s, password:%s",
             CONFIG_ESP_WIFI_SSID,
             CONFIG_ESP_WIFI_PASSWORD);
  } else {
    ESP_LOGE(TAG, "Unexpected event occurred.");
  }
}
