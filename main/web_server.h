#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include "esp_http_server.h"

// Зовнішні змінні зі статусом Zigbee
extern uint16_t pan_id;
extern uint8_t channel;
extern uint16_t short_addr;

// Структура для пристроїв
#define MAX_DEVICES 10
typedef struct {
    uint16_t short_addr;
    char name[20];
} zb_device_t;

extern zb_device_t devices[MAX_DEVICES];
extern int device_count;

// Функції
esp_err_t web_handler(httpd_req_t *req);
esp_err_t css_handler(httpd_req_t *req);
esp_err_t js_handler(httpd_req_t *req);
esp_err_t api_status_handler(httpd_req_t *req);
esp_err_t api_permit_join_handler(httpd_req_t *req);
esp_err_t favicon_handler(httpd_req_t *req);
void start_web_server(void);
void add_device(uint16_t addr);

#endif