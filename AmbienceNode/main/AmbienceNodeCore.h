#ifndef AMBIENCENODECORE_H_INCLUDED
#define AMBIENCENODECORE_H_INCLUDED

// C Standard Library
#include <stdio.h>
#include "string.h"

// FreeRTOS
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"

// ESP-IDF
#include "sdkconfig.h"
#include "esp_efuse.h"
#include "esp_event.h"
#include "esp_http_client.h"
#include "esp_https_ota.h"
#include "esp_log.h"
#include "esp_ota_ops.h"
#include "esp_spi_flash.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "protocol_examples_common.h"

// Logging
void core_boot_log();

#endif