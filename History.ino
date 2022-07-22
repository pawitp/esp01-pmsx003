// Save in 6 seconds increments up to 4 hours
#define HISTORY_INCREMENT_SEC 6
#define HISTORY_SIZE ((60 / HISTORY_INCREMENT_SEC) * 60 * 4)

unsigned char historyAqi[HISTORY_SIZE];
short historyTemperature[HISTORY_SIZE];
short historyHumidity[HISTORY_SIZE];

void historyInit() {
  for (int i = 0; i < HISTORY_SIZE; i++) {
    historyAqi[i] = -1;
  }
}

void historySave(int pm25, int temperature, int humidity) {
  unsigned char bytePm25 = 254; // Max is 254 since 255 (-1) is used as no data
  if (pm25 < 254) {
    bytePm25 = (unsigned char) pm25;
  }
  int slot = historySlot();
  historyAqi[slot] = bytePm25;
  historyTemperature[slot] = temperature;
  historyHumidity[slot] = humidity;
}

int historySlot() {
  int slot = millis() / (HISTORY_INCREMENT_SEC * 1000);

  if (slot >= HISTORY_SIZE) {
    slot = HISTORY_SIZE - 1;
  }

  return slot;
}
