#include "Screen/Screen.h"
U8G2_SSD1306_128X64_NONAME_F_HW_I2C g_display(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
uint8_t getPixel(uint16_t x, uint16_t y, uint8_t *dest_ptr, uint8_t tile_width);

void Screen::begin() {
    g_display.begin();

}

void Screen::getPixels(string &pixels){
  Serial.print("getPixels on Core ");
  Serial.println(xPortGetCoreID());
  readPixels(getPixel, pixels);
  
}

void Screen::readPixels(uint8_t (*get_pixel)(uint16_t x, uint16_t y, uint8_t *dest_ptr, uint8_t tile_width), string & pixels) {
  uint16_t x, y;
  uint16_t w, h;
  uint8_t *buffer = g_display.getBufferPtr();
  uint8_t tile_width = g_display.getBufferTileWidth();
  w = g_display.getBufferTileWidth();
  w *= 8;
  h = g_display.getBufferTileHeight();
  h *= 8;
  pixels = "";

  for (y = 0; y < h; y++) {
    for (x = 0; x < w; x++) {
      if (get_pixel(x, y, buffer, tile_width)){
        pixels.append("1");
      }
      else {
        pixels.append("0");
      }
    }
  }
}
uint8_t getPixel(uint16_t x = 0, uint16_t y = 0,
                    uint8_t *dest_ptr = g_display.getBufferPtr(),
                    uint8_t tile_width = g_display.getBufferTileWidth()) {
  dest_ptr += (y / 8) * tile_width * 8;
  y &= 7;
  dest_ptr += x;
  if ((*dest_ptr & (1 << y)) == 0) return 0;
  return 1;
}
Screen::Screen() {}
Screen::~Screen() {}