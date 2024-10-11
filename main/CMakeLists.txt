idf_component_register(
    SRCS
        "src/wifi.h"

        "src/ps2keyboard.c"
        "src/wifi.c"
    REQUIRES
        esp_wifi
        nvs_flash
    INCLUDE_DIRS
        "."
)