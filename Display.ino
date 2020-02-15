#include <Wire.h>
#include <SSD1306Wire.h>

#define OVERLAY_DURATION 5000

long overlayExpire = 0;

SSD1306Wire display(0x3c, 0, 2);

void displayInit() {
  display.init();
  display.flipScreenVertically();
}

void displayText(String txt) {
  if (millis() < overlayExpire) return;

  display.clear();
  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawStringMaxWidth(0, 0, 128, txt);
  display.display();
}

void displayTextOverlay(String txt) {
  overlayExpire = millis() + OVERLAY_DURATION;

  display.clear();
  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawStringMaxWidth(0, 0, 128, txt);
  display.display();
}

void displayAqi(int pm25, int aqi25) {
  if (millis() < overlayExpire) return;

  display.clear();
  display.setFont(ArialMT_Plain_24);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(0, 0, "PM2.5");
  display.drawString(0, 32, "AQI");
  display.setTextAlignment(TEXT_ALIGN_RIGHT);
  display.drawString(128, 0, String(pm25));
  display.drawString(128, 32, String(aqi25));
  display.display();
}

void displayFirmwareProgress(int progress, int total) {
  overlayExpire = millis() + OVERLAY_DURATION;

  display.clear();
  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(display.getWidth() / 2, 0, "Firmware Update");
  display.drawProgressBar(4, 32, 120, 8, progress / (total / 100));
  display.display();
}
