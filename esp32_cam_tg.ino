#include <Wire.h>

#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

#define FLASH_LED_PIN 4


int botRequestDelay = 1000;
unsigned long lastTimeBotRun;
bool sendPhoto = false;
bool flashState = LOW;

void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
  Serial.begin(115200);

  // что то на еспешном
  pinMode(FLASH_LED_PIN, OUTPUT);
  digitalWrite(FLASH_LED_PIN, flashState);

  setup_camera();
  setup_wifi();
  setup_telegram();
}

void loop() {
  if (sendPhoto) {
    Serial.println("Preparing photo");
    sendPhotoTelegram();
    sendPhoto = false;
  }
  if (millis() > lastTimeBotRun + botRequestDelay) {
    Serial.println("Update");
    get_updates_telegram();
    lastTimeBotRun = millis();
  }
}
