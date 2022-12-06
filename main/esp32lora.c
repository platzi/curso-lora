#include <stdio.h>
#include "string.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "ssd1306.h"
#include "lora.h"

SSD1306_t screen;

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
  for(;;) {
    
  }
}

void task_tx(void *p) {
  for(;;) {
    
  }
}

void lora_config_init() {
  printf("lora config init!\n");
  lora_init();
  lora_set_frequency(915e6);
  lora_enable_crc();
  xTaskCreate(&task_rx, "task_rx", 2048, NULL, 5, NULL);
  xTaskCreate(&task_tx, "task_tx", 2048, NULL, 5, NULL);
}

void app_main(void) {
  screen_init();
  screen_clear();
  screen_print("Hola mundo!", 0);
}
