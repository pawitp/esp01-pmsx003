#define TIMEOUT 3000

unsigned long lastUpdate = 0;

void setup(void) {
  // We use serial TX for debugging and RX for receiving measurements
  Serial.begin(9600, SERIAL_8N1);

  // Setup display
  displayInit();
  displayText("Starting...");

  // Setup history
  historyInit();

  // Setup Wi-Fi
  wifiInit();

  // Setup NTP client
  ntpInit();

  lastUpdate = millis();

  Serial.println("Started");
}

void loop() {
  sensorLoop();
  wifiLoop();

  if (isTimeout()) {
    displayText("No data");
  }
}

void onSensorError(String error) {
  displayText(error);
  lastUpdate = millis();
}

void onSensorRead(int pm25) {
  int aqi25 = calculateAqi(pm25);

  Serial.println("AQI");
  Serial.println(pm25);
  Serial.println(aqi25);

  displayAqi(pm25, aqi25);
  historySave(pm25);
  lastUpdate = millis();
}

bool isTimeout() {
  return millis() - lastUpdate > TIMEOUT;
}
