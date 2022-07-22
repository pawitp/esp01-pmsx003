#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

wl_status_t previousStatus = WL_IDLE_STATUS;
ESP8266WebServer server(80);

const char* serverIndex = "<a href='history'>PM 2.5 History</a><br><a href='wifi'>Wi-Fi setup</a><br><a href='softap'>SoftAP setup</a><br><a href='update'>Firmware Update</a>";
const char* serverUpdate = "Firmware Update <form method='POST' action='/update' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' value='Update'></form>";
const char* serverWifi = "Wi-Fi setup <form method='POST' action='/wifi'>SSID: <input type='input' name='ssid'><br>Password: <input type='password' name='password'><br><input type='submit' value='Update'></form>";
const char* serverSoftAp = "SoftAP setup <form method='POST' action='/wifi'>SSID: <input type='input' name='ssid'><br>Password: <input type='password' name='password'><br><input type='submit' value='Update'><input type='hidden' name='mode' value='softap'></form>";

void wifiInit() {
  WiFi.mode(WIFI_AP_STA);

  // Index
  server.on("/", HTTP_GET, []() {
    String response = "Current PM2.5: <b>";
    if (isTimeout()) {
      response += "No data";
    } else {
      response += pm25;
    }
    response += "</b><br><br>";
    response += serverIndex;
    server.send(200, "text/html", response);
  });

  // History Server
  server.on("/history", HTTP_GET, []() {
    server.setContentLength(CONTENT_LENGTH_UNKNOWN);
    server.send(200, "text/plain", "");

    int currentSlot = historySlot();
    for (int i = 0; i <= currentSlot; i++) {
      // No data for current slot yet
      if (i == currentSlot && historyAqi[i] == 255) continue;

      int timeForSlot = timeDiff + i * HISTORY_INCREMENT_SEC;
      String line = String(timeForSlot);
      line += ",";
      line += historyAqi[i];
      line += ",";
      line += historyTemperature[i];
      line += ",";
      line += historyHumidity[i];
      line += "\n";
      server.sendContent(line);
    }
    // Terminate the HTTP chunked transmission with a 0-length chunk
    server.sendContent("");
  });

  // Wi-Fi setup
  server.on("/wifi", HTTP_GET, []() {
    server.send(200, "text/html", serverWifi);
  });
  server.on("/wifioff", HTTP_GET, []() {
  WiFi.mode(WIFI_OFF);
  WiFi.forceSleepBegin();
    server.send(200, "text/html", "OK");
  });
  server.on("/softap", HTTP_GET, []() {
    server.send(200, "text/html", serverSoftAp);
  });
  server.on("/wifi", HTTP_POST, []() {
    String ssid = "";
    String password = "";
    String mode = "";
    for (uint8_t i = 0; i < server.args(); i++) {
      String name = server.argName(i);
      if (name == "ssid") {
        ssid = server.arg(i);
      }
      if (name == "password") {
        password = server.arg(i);
      }
      if (name == "mode") {
        mode = server.arg(i);
      }
    }

    if (mode == "softap") {
      Serial.println("Updated SoftAP settings");
      displayTextOverlay("Updated SoftAP settings");
      WiFi.softAP(ssid, password);
      server.send(200, "text/html", "Updated SoftAP settings <a href='/'>Home</a>");
    }
    else {
      Serial.println("Updated Wi-Fi settings");
      displayTextOverlay("Updated Wi-Fi settings");
      WiFi.begin(ssid, password);
      server.send(200, "text/html", "Updated Wi-Fi settings <a href='/'>Home</a>");
    }
  });

  // OTA firmware
  server.on("/update", HTTP_GET, []() {
    server.send(200, "text/html", serverUpdate);
  });
  server.on("/update", HTTP_POST, []() {
    server.send(200, "text/html", (Update.hasError()) ? "FAIL <a href='/'>Home</a>" : "OK <a href='/'>Home</a>");
    delay(2000);
    ESP.restart();
  }, []() {
    HTTPUpload& upload = server.upload();
    if (upload.status == UPLOAD_FILE_START) {
      Serial.setDebugOutput(true);
      Serial.printf("Update: %s\n", upload.filename.c_str());
      uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
      displayFirmwareProgress(0, 100);

      if (!Update.begin(maxSketchSpace)) { //start with max available size
        displayTextOverlay("Firmware Update Failed");
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      displayFirmwareProgress(upload.totalSize, upload.contentLength);

      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
        displayTextOverlay("Firmware Update Failed");
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_END) {
      if (Update.end(true)) { //true to set the size to the current progress
        displayTextOverlay("Firmware Update Success");
        Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
      } else {
        displayTextOverlay("Firmware Update Failed");
        Update.printError(Serial);
      }
      Serial.setDebugOutput(false);
    }
    yield();
  });
  server.begin();
}

void wifiLoop() {
  server.handleClient();

  wl_status_t currentStatus = WiFi.status();
  if (currentStatus != previousStatus) {
    if (currentStatus == WL_CONNECTED) {
      String status = "Connected to ";
      status += WiFi.SSID();
      status += "\nIP: ";
      status += WiFi.localIP().toString();

      Serial.println(status);
      displayTextOverlay(status);
    }
    else if (currentStatus == WL_CONNECT_FAILED) {
      String status = "Connection to ";
      status += WiFi.SSID();
      status += " failed";

      Serial.println(status);
      displayTextOverlay(status);
    }
    else {
      String status = "Wi-Fi status is ";
      status += currentStatus;

      Serial.println(status);
    }
  }

  previousStatus = currentStatus;
}
