#include <WiFi.h>


#define WIFI_SSID "***"
#define WIFI_PASS "***"

void setup_wifi(){
  WiFi.mode(WIFI_STA);
  Serial.println("Setup Wifi start");
  Serial.println("Connecting to ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("ESP32-CAM IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.println("Setup Wifi succesful");
}