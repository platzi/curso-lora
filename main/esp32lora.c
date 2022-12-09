#include <stdio.h>
#include "string.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "ssd1306.h"
#include "lora.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "protocol_examples_common.h"
#include "esp_http_server.h"

#define MESSAGE_LENGTH 240

extern const char index_start[] asm("_binary_index_html_start");
extern const char index_end[] asm("_binary_index_html_end");
extern const char logo_start[] asm("_binary_logo_png_start");
extern const char logo_end[] asm("_binary_logo_png_end");

SSD1306_t screen;
TaskHandle_t xHandleRXTask;
uint8_t msg[MESSAGE_LENGTH];
int packets = 0;
int rssi = 0;

void screen_init() {
  i2c_master_init(&screen, CONFIG_SDA_GPIO, CONFIG_SCL_GPIO, CONFIG_RESET_GPIO);
  ssd1306_init(&screen, 128, 64);
  ssd1306_contrast(&screen, 0xFF);
}

void screen_clear() {
  ssd1306_clear_screen(&screen, false);
}

void screen_print(char * str, int page) {
  ssd1306_clear_line(&screen, page, false);
  ssd1306_display_text(&screen, page, str, strlen(str), false);
}

void task_rx(void *p) {
  char packets_count[64];
  char rssi_str[64];
  int len;
  for(;;) {
    lora_receive();
    while(lora_received()) {
      len = lora_receive_packet(msg, MESSAGE_LENGTH);
      msg[len] = 0;

      printf("Receive msg: %s, len: %d\n", msg, len);

      rssi = lora_packet_rssi();
      packets++;

      sprintf(packets_count, "Count: %d", packets);
      sprintf(rssi_str, "RSSI: %d dBm", rssi);
      screen_print(packets_count, 6);
      screen_print(rssi_str, 4);
    }
    vTaskDelay(1);
  }
}

void send_msg(char * msg, int size) {
  printf("send packet: %s\n", msg);
  vTaskSuspend(xHandleRXTask);
  lora_send_packet((uint8_t *)msg, size);
  vTaskResume(xHandleRXTask);
  printf("packet sent!\n");
}

void task_tx(void *p) {
  for(;;) {
    send_msg("Ping!", 5);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }
}

void lora_config_init() {
  printf("lora config init!\n");
  lora_init();
  lora_set_frequency(915e6);
  lora_enable_crc();
  xTaskCreate(&task_rx, "task_rx", 2048, NULL, 5, &xHandleRXTask);
  // xTaskCreate(&task_tx, "task_tx", 2048, NULL, 5, NULL);
}

static esp_err_t home_get_handler(httpd_req_t *req) {
  httpd_resp_set_type(req, "text/html");

  const uint32_t index_len = index_end - index_start;
  httpd_resp_send(req, index_start, index_len);

  return ESP_OK;
}

static esp_err_t logo_get_handler(httpd_req_t *req) {
  httpd_resp_set_type(req, "image/png");

  const uint32_t logo_len = logo_end - logo_start;
  httpd_resp_send(req, logo_start, logo_len);

  return ESP_OK;
}

static esp_err_t api_message_get_handler(httpd_req_t *req) {
  httpd_resp_set_type(req, "application/json");

  // { "message": "" }
  char res[400] = "{\"message\":\"\"}";
  sprintf(res, "{\"message\":\"%s\"}", msg);

  httpd_resp_send(req, res, HTTPD_RESP_USE_STRLEN);

  msg[0] = 0;

  return ESP_OK;
}

static esp_err_t api_send_get_handler(httpd_req_t *req) {
  httpd_resp_set_type(req, "application/json");

  char* buff;
  size_t buf_len;

  buf_len = httpd_req_get_url_query_len(req) + 1;

  if (buf_len > 1) {
    buff = malloc(buf_len);
    // api/send?msg=hello
    if (httpd_req_get_url_query_str(req, buff, buf_len) == ESP_OK) {
      char param[240];
      if (httpd_query_key_value(buff, "msg", param, sizeof(param)) == ESP_OK) {
        printf("msg: %s\n", (char *)msg);
        send_msg((char *)param, sizeof(param));
      }
    }
    free(buff);
  }

  char res[400] = "{\"message\":\"\"}";
  sprintf(res, "{\"message\":\"%s\"}", msg);

  httpd_resp_send(req, res, HTTPD_RESP_USE_STRLEN);

  msg[0] = 0;

  return ESP_OK;
}

static const httpd_uri_t home = {
  .uri = "/",
  .method = HTTP_GET,
  .handler = home_get_handler
};

static const httpd_uri_t logo = {
  .uri = "/logo.png",
  .method = HTTP_GET,
  .handler = logo_get_handler
};

static const httpd_uri_t api_message = {
  .uri = "/api/message",
  .method = HTTP_GET,
  .handler = api_message_get_handler
};

static const httpd_uri_t api_send = {
  .uri = "/api/send",
  .method = HTTP_GET,
  .handler = api_send_get_handler
};

void web_server_init() {
  httpd_handle_t server = NULL;
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();

  if (httpd_start(&server, &config) == ESP_OK) {
    httpd_register_uri_handler(server, &home);
    httpd_register_uri_handler(server, &logo);
    httpd_register_uri_handler(server, &api_message);
    httpd_register_uri_handler(server, &api_send);
    return;
  }

  printf("Error al iniciar servidor\n");
}


void app_main(void) {
  screen_init();
  screen_clear();

  // screen_print("Hola mundo!", 0);
  lora_config_init();

  nvs_flash_init();
  esp_netif_init();
  esp_event_loop_create_default();
  example_connect();

  esp_netif_ip_info_t ip_info;
  esp_netif_t *netif = NULL;
  netif = esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");

  if (netif == NULL) {
    printf("No hay interfaz\n");
  } else {
    wifi_config_t conf;

    if (esp_wifi_get_config(WIFI_IF_STA, &conf) == ESP_OK) {
      printf("WiFi: %s\n", (char*)conf.sta.ssid);
      char ssid[64];
      sprintf(ssid, "%s\n", (char *)conf.sta.ssid);
      screen_print(ssid, 0);
    }

    if (esp_netif_get_ip_info(netif, &ip_info) == ESP_OK) {
      printf("IP: %d.%d.%d.%d\n", IP2STR(&ip_info.ip)); 
      char ip[64];
      sprintf(ip, "%d.%d.%d.%d\n", IP2STR(&ip_info.ip));
      screen_print(ip, 2);
    }

    screen_print("RSSI: -- dBm", 4);
    screen_print("Count: 0", 6);

    web_server_init();
  }
}
