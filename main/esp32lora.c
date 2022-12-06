#include <stdio.h>
#include "string.h"
#include "ssd1306.h"

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

void app_main(void) {
  screen_init();
  screen_clear();
  screen_print("Hola mundo!", 0);
}
