#define BUFFER_LEN 32

// For the bytes we read from the serial port
byte checkHigh = 0;
int check = 0;
int pos = 0;
int pm25 = 0;
int temperature = 0;
int humidity = 0;

void sensorLoop() {
  while (Serial.available()) {
    byte data = Serial.read();

    // This is a state machine based on current length of data
    if (pos == 0) {
      if (data == 66) {
        // Data start found
        pos = 1;
        check = data;
        pm25 = 0;
        temperature = 0;
        humidity = 0;
      }
    } else if (pos == 1) {
      if (data == 77) {
        pos++;
        check += data;
      } else {
        Serial.printf("Early E %d %d\n", check, data);
        pos = 0;

        onSensorError("Error 2");
      }
    } else if (pos == 12) {
      pm25 += data * 256;
      pos++;
      check += data;
    } else if (pos == 13) {
      pm25 += data;
      pos++;
      check += data;
    } else if (pos == 24) {
      temperature += data * 256;
      pos++;
      check += data;
    } else if (pos == 25) {
      temperature += data;
      pos++;
      check += data;
    } else if (pos == 26) {
      humidity += data * 256;
      pos++;
      check += data;
    } else if (pos == 27) {
      humidity += data;
      pos++;
      check += data;
    } else if (pos == BUFFER_LEN - 2) {
      checkHigh = data;
      pos++;
    } else if (pos == BUFFER_LEN - 1) {
      byte expCheckHigh = check / 256;
      byte expCheckLow = check % 256;
      pos = 0;

      if (checkHigh == expCheckHigh && data == expCheckLow) {
        onSensorRead(pm25, temperature, humidity);
      } else {
        Serial.printf("Checksum Error\n%d %d %d %d\n", checkHigh, data, expCheckHigh, expCheckLow);
        onSensorError("Checksum Error");
      }
    } else {
      pos++;
      check += data;
    }
  }
}
