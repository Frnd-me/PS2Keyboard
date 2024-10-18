idf_component_register(
    SRCS
        "src/server.h"
        "src/wifi.h"

        "src/ps2keyboard.c"
        "src/server.c"
        "src/wifi.c"
    REQUIRES
        esp_wifi
        nvs_flash
    INCLUDE_DIRS
        "."
)