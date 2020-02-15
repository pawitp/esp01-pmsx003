// Save in 6 seconds increments up to 1 day
#define HISTORY_INCREMENT_SEC 6
#define HISTORY_SIZE ((60 / HISTORY_INCREMENT_SEC) * 60 * 24)

unsigned char history[10 * 60 * 24];

void historyInit() {
  for (int i = 0; i < HISTORY_SIZE; i++) {
    history[i] = -1;
  }
}

void historySave(int pm25) {
  unsigned char bytePm25 = 254; // Max is 254 since 255 (-1) is used as no data
  if (pm25 < 254) {
    bytePm25 = (unsigned char) pm25;
  }
  history[historySlot()] = bytePm25;
}

int historySlot() {
  int slot = millis() / (HISTORY_INCREMENT_SEC * 1000);

  if (slot >= HISTORY_SIZE) {
    slot = HISTORY_SIZE - 1;
  }

  return slot;
}
