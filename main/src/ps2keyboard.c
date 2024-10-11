#include "ps2keyboard.h"
#include "wifi.h"

#include "esp_log.h"
#include "nvs_flash.h"

// Main application entry point
void
app_main(void)
{
  // Initialize the Non-Volatile Storage (NVS)
  esp_err_t ret = nvs_flash_init();

  // Check if the initialization failed due to lack of free pages or new NVS
  // version
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
      ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    // If so, erase NVS and reinitialize it
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }

  // Check for any other errors during NVS initialization
  ESP_ERROR_CHECK(ret);

  // Initialize Wi-Fi in station mode (connect to an Access Point)
  wifi_init_sta();
}