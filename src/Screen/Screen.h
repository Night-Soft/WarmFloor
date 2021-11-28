#ifndef Screen_H
#define Screen_H

#include <Arduino.h>
#include <U8g2lib.h>
using namespace std;

extern U8G2_SSD1306_128X64_NONAME_F_HW_I2C g_display;

class Screen {
 private:
  /* data */
  void readPixels(uint8_t (*get_pixel)(uint16_t x, uint16_t y,
                                         uint8_t *dest_ptr, uint8_t tile_width),
                    string &pixels);

 public:
  void begin();
  void pumpOf();
  void testShow();
  void getPixels(string &pixels);
 // string getPixels(string pixels = "noPixel");

 public:
  Screen();
  ~Screen();
};

#endif