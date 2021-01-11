#include "AmbienceNodeOTA.h"

static const char* OTA_TAG = "OTA";
static esp_err_t ota_validate_image_header(esp_app_desc_t *new_app_info);
static esp_err_t ota_http_client_init_cb(esp_http_client_handle_t http_client);

extern const uint8_t server_cert_pem_start[] asm("_binary_ca_cert_pem_start");
extern const uint8_t server_cert_pem_end[] asm("_binary_ca_cert_pem_end");

esp_err_t ota_validate_image_header(esp_app_desc_t *new_app_info)
{
	if (new_app_info == NULL) 
	{
        return ESP_ERR_INVALID_ARG;
    }

    const esp_partition_t *running = esp_ota_get_running_partition();
    esp_app_desc_t running_app_info;
    if (esp_ota_get_partition_description(running, &running_app_info) == ESP_OK) 
	{
        ESP_LOGI(OTA_TAG, "running firmware version: %s", running_app_info.version);
    }

#ifndef CONFIG_EXAMPLE_SKIP_VERSION_CHECK
    if (memcmp(new_app_info->version, running_app_info.version, sizeof(new_app_info->version)) == 0) 
	{
        ESP_LOGW(OTA_TAG, "the currently running firmware is already up to date.");
        return ESP_FAIL;
    }
#endif
    return ESP_OK;
}


esp_err_t ota_http_client_init_cb(esp_http_client_handle_t http_client)
{
	esp_err_t e = ESP_OK;
    /* Uncomment to add custom headers to HTTP request */
    // err = esp_http_client_set_header(http_client, "Custom-Header", "Value");
    return e;
}


void ota_task(void *pvParameter)
{
	ESP_LOGI(OTA_TAG, "starting OTA update task");

    esp_http_client_config_t config = 
	{
        .url = CONFIG_EXAMPLE_FIRMWARE_UPGRADE_URL,
        .cert_pem = (char *)server_cert_pem_start,
        .timeout_ms = CONFIG_EXAMPLE_OTA_RECV_TIMEOUT,
    };

#ifdef CONFIG_EXAMPLE_FIRMWARE_UPGRADE_URL_FROM_STDIN
    char url_buf[OTA_URL_SIZE];
    if (strcmp(config.url, "FROM_STDIN") == 0) 
	{
        example_configure_stdin_stdout();
        fgets(url_buf, OTA_URL_SIZE, stdin);
        int len = strlen(url_buf);
        url_buf[len - 1] = '\0';
        config.url = url_buf;
    } 
	else 
	{
        ESP_LOGE(OTA_TAG, "configuration mismatch: wrong firmware upgrade image url");
        abort();
    }
#endif

#ifdef CONFIG_EXAMPLE_SKIP_COMMON_NAME_CHECK
    config.skip_cert_common_name_check = true;
#endif

	esp_https_ota_config_t ota_config = 
	{
		.http_config = &config,
		.http_client_init_cb = ota_http_client_init_cb, // Register a callback to be invoked after esp_http_client is initialized
	};

	esp_err_t e;
	esp_err_t ota_finish_e = ESP_OK;
	esp_https_ota_handle_t https_ota_handle = NULL;

    e = esp_https_ota_begin(&ota_config, &https_ota_handle);
    if (e != ESP_OK) 
	{
        ESP_LOGE(OTA_TAG, "esp_https_ota_begin failed");
        vTaskDelete(NULL);
    }
	
	esp_app_desc_t app_desc;
    e = esp_https_ota_get_img_desc(https_ota_handle, &app_desc);
    if (e != ESP_OK) 
	{
        ESP_LOGE(OTA_TAG, "esp_https_ota_read_img_desc failed");
        goto ota_end;
	}

	e = ota_validate_image_header(&app_desc);
    if (e != ESP_OK) 
	{
        ESP_LOGE(OTA_TAG, "ota_validate_image_header failed");
        goto ota_end;
    }

	while (1) 
	{
        e = esp_https_ota_perform(https_ota_handle);
        if (e != ESP_ERR_HTTPS_OTA_IN_PROGRESS) { break; }

        // esp_https_ota_perform returns after every read operation which gives user the ability to
        // monitor the status of OTA upgrade by calling esp_https_ota_get_image_len_read, which gives length of image
        // data read so far.
        ESP_LOGD(OTA_TAG, "image bytes read: %d", esp_https_ota_get_image_len_read(https_ota_handle));
    }

    if (esp_https_ota_is_complete_data_received(https_ota_handle) != true) 
	{
        // the OTA image was not completely received and user can customise the response to this situation.
        ESP_LOGE(OTA_TAG, "an error occured while uploading OTA image");
    }

ota_end:
	ota_finish_e = esp_https_ota_finish(https_ota_handle);
	if ((e == ESP_OK) && (ota_finish_e == ESP_OK)) 
	{
        ESP_LOGI(OTA_TAG, "ESP_HTTPS_OTA upgrade successful. rebooting ...");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        esp_restart();
	}
	else
	{
		if (ota_finish_e == ESP_ERR_OTA_VALIDATE_FAILED) 
		{
            ESP_LOGE(OTA_TAG, "image validation failed, image is corrupted");
        }
        ESP_LOGE(OTA_TAG, "OTA upgrade failed %d", ota_finish_e);
        vTaskDelete(NULL);
	}
}