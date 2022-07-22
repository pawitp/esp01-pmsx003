#include <Wire.h>
#include <SSD1306Wire.h>

#define OVERLAY_DURATION 5000

long overlayExpire = 0;
boolean beep = false;

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

void displayAqi(int pm25, int aqi25, int rawTemperature, int rawHumidity) {
  if (millis() < overlayExpire) return;

  beep = !beep;

  display.clear();
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(0, 3, "PM");
  display.drawString(0, 13, "2.5");
  display.drawString(0, 40, "AQI");

  display.setTextAlignment(TEXT_ALIGN_RIGHT);
  display.drawString(128, 10, "C");
  display.drawString(128, 42, "%");

  display.setFont(ArialMT_Plain_24);
  display.drawString(60, 0, String(pm25));
  display.drawString(60, 32, String(aqi25));
  display.drawString(120, 0, String(rawTemperature / 10.0F, 1));
  display.drawString(120, 32, String(rawHumidity / 10.0F, 1));

  if (beep) {
    display.drawLine(64 - 4, 63, 64 + 4, 63);
  }

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
