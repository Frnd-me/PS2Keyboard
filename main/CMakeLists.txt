idf_component_register(
    SRCS
        "src/main.h"
        "src/ps2keyboard.h"
        "src/scancode.h"
        "src/server.h"
        "src/wifi.h"

        "src/main.c"
        "src/ps2keyboard.c"
        "src/server.c"
        "src/wifi.c"
    REQUIRES
        esp_wifi
        nvs_flash
    INCLUDE_DIRS
        "."
)