#include "web_server.h"
#include "esp_log.h"
#include "esp_zigbee_gateway.h"
#include <string.h>

static const char *TAG = "WEB_SERVER";

zb_device_t devices[MAX_DEVICES];
int device_count = 0;

void add_device(uint16_t addr) {
    if (device_count < MAX_DEVICES) {
        devices[device_count].short_addr = addr;
        snprintf(devices[device_count].name, sizeof(devices[device_count].name), "Device 0x%04x", addr);
        device_count++;
    }
}

// Frontend: HTML з посиланнями на CSS/JS
esp_err_t web_handler(httpd_req_t *req)
{
    FILE* f = fopen("/www/index.html", "r");
    if (!f) {
        httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "File not found");
        return ESP_FAIL;
    }
    char buf[2048];
    size_t len = fread(buf, 1, sizeof(buf), f);
    fclose(f);
    httpd_resp_send(req, buf, len);
    return ESP_OK;
}

// Обробник для CSS
esp_err_t css_handler(httpd_req_t *req)
{
    FILE* f = fopen("/www/style.css", "r");
    if (!f) {
        httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "File not found");
        return ESP_FAIL;
    }
    char buf[1024];
    size_t len = fread(buf, 1, sizeof(buf), f);
    fclose(f);
    httpd_resp_set_type(req, "text/css");
    httpd_resp_send(req, buf, len);
    return ESP_OK;
}

// Обробник для JS
esp_err_t js_handler(httpd_req_t *req)
{
    FILE* f = fopen("/www/script.js", "r");
    if (!f) {
        httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "File not found");
        return ESP_FAIL;
    }
    char buf[2048];
    size_t len = fread(buf, 1, sizeof(buf), f);
    fclose(f);
    httpd_resp_set_type(req, "application/javascript");
    httpd_resp_send(req, buf, len);
    return ESP_OK;
}

// API: Статус у JSON
esp_err_t api_status_handler(httpd_req_t *req)
{
    char json[1024];
    int offset = 0;
    offset += snprintf(json + offset, sizeof(json) - offset, "{\"pan_id\":%u,\"channel\":%u,\"short_addr\":%u,\"devices\":[", pan_id, channel, short_addr);
    for (int i = 0; i < device_count; i++) {
        offset += snprintf(json + offset, sizeof(json) - offset, "{\"name\":\"%s\"}%s", devices[i].name, (i < device_count - 1) ? "," : "");
    }
    offset += snprintf(json + offset, sizeof(json) - offset, "]}");
    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, json, strlen(json));
    return ESP_OK;
}

// API: Permit Join
esp_err_t api_permit_join_handler(httpd_req_t *req)
{
    esp_zb_bdb_open_network(60);
    const char* resp = "{\"message\":\"Network opened for 60 seconds\"}";
    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, resp, strlen(resp));
    return ESP_OK;
}

esp_err_t favicon_handler(httpd_req_t *req)
{
    httpd_resp_set_status(req, "204 No Content");
    httpd_resp_send(req, NULL, 0);
    return ESP_OK;
}

void start_web_server(void)
{
    httpd_handle_t server = NULL;
    httpd_config_t httpd_config = HTTPD_DEFAULT_CONFIG();
    httpd_config.server_port = 80;
    ESP_ERROR_CHECK(httpd_start(&server, &httpd_config));

    httpd_uri_t uri_get = {
        .uri       = "/",
        .method    = HTTP_GET,
        .handler   = web_handler,
        .user_ctx  = NULL
    };
    httpd_register_uri_handler(server, &uri_get);

    httpd_uri_t uri_css = {
        .uri       = "/style.css",
        .method    = HTTP_GET,
        .handler   = css_handler,
        .user_ctx  = NULL
    };
    httpd_register_uri_handler(server, &uri_css);

    httpd_uri_t uri_js = {
        .uri       = "/script.js",
        .method    = HTTP_GET,
        .handler   = js_handler,
        .user_ctx  = NULL
    };
    httpd_register_uri_handler(server, &uri_js);

    httpd_uri_t uri_api_status = {
        .uri       = "/api/status",
        .method    = HTTP_GET,
        .handler   = api_status_handler,
        .user_ctx  = NULL
    };
    httpd_register_uri_handler(server, &uri_api_status);

    httpd_uri_t uri_api_permit = {
        .uri       = "/api/permit_join",
        .method    = HTTP_POST,
        .handler   = api_permit_join_handler,
        .user_ctx  = NULL
    };
    httpd_register_uri_handler(server, &uri_api_permit);

    httpd_uri_t uri_favicon = {
        .uri       = "/favicon.ico",
        .method    = HTTP_GET,
        .handler   = favicon_handler,
        .user_ctx  = NULL
    };
    httpd_register_uri_handler(server, &uri_favicon);

    ESP_LOGI(TAG, "Web server started on port 80");
}