#include <stdio.h>
#include "ssd1306.h"

SSD1306_t screen;

void app_main(void) {
  i2c_master_init(&screen, CONFIG_SDA_GPIO, CONFIG_SCL_GPIO, CONFIG_RESET_GPIO);
  ssd1306_init(&screen, 128, 64);
  ssd1306_clear_screen(&screen, false);
  ssd1306_contrast(&screen, 0xFF);
  ssd1306_clear_line(&screen, 0, false);
  ssd1306_display_text(&screen, 0, "Hola mundo!", 11, false);
}
