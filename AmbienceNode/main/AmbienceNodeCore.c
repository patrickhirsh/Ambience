#include "AmbienceNodeCore.h"

static const char* CORE_TAG = "CORE";


void core_boot_log()
{
	ESP_LOGI(CORE_TAG, "starting AmbienceNode...");
	
	esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
	ESP_LOGI(CORE_TAG, "  detected hardware:       %s", CONFIG_IDF_TARGET);
	ESP_LOGI(CORE_TAG, "  core(s):                 %d", chip_info.cores);
	ESP_LOGI(CORE_TAG, "  silicon revision:        %d", chip_info.revision);
	ESP_LOGI(CORE_TAG, "  flash:                   %dMB %s", spi_flash_get_chip_size() / (1024 * 1024), (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");
	ESP_LOGI(CORE_TAG, "  minimum free heap size:  %d bytes", esp_get_minimum_free_heap_size());
}