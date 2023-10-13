#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>

String BOTtoken = "***";
String chatId = "***";

int waitTime = 10000;

WiFiClientSecure clientTCP;
UniversalTelegramBot bot(BOTtoken, clientTCP);


void setup_telegram(){
  Serial.println("Setup telegram start");
  clientTCP.setCACert(TELEGRAM_CERTIFICATE_ROOT);
  Serial.println("Setup telegram succesful");
}


String sendPhotoTelegram() {
  // Делает и отправляет фотографию
  const char* myDomain = "api.telegram.org";
  String getAll = "";
  String getBody = "";

  camera_fb_t * fb = NULL;
  fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Camera capture failed");
    delay(1000);
    ESP.restart();
    return "Camera capture failed";
  }

  Serial.println("Connect to " + String(myDomain));

  if (clientTCP.connect(myDomain, 443)) {
    Serial.println("Connection successful");

    String head = "--RandomNerdTutorials\r\nContent-Disposition: form-data; name=\"chat_id\"; \r\n\r\n" + chatId + "\r\n--RandomNerdTutorials\r\nContent-Disposition: form-data; name=\"photo\"; filename=\"esp32-cam.jpg\"\r\nContent-Type: image/jpeg\r\n\r\n";
    String tail = "\r\n--RandomNerdTutorials--\r\n";

    uint16_t imageLen = fb->len;
    uint16_t extraLen = head.length() + tail.length();
    uint16_t totalLen = imageLen + extraLen;

    clientTCP.println("POST /bot" + BOTtoken + "/sendPhoto HTTP/1.1");
    clientTCP.println("Host: " + String(myDomain));
    clientTCP.println("Content-Length: " + String(totalLen));
    clientTCP.println("Content-Type: multipart/form-data; boundary=RandomNerdTutorials");
    clientTCP.println();
    clientTCP.print(head);
    Serial.println(totalLen);

    uint8_t *fbBuf = fb->buf;
    size_t fbLen = fb->len;
    
    for (size_t n = 0; n < fbLen; n = n + 1024) {
      if (n + 1024 < fbLen) {
        clientTCP.write(fbBuf, 1024);
        fbBuf += 1024;
      }
      else if (fbLen % 1024 > 0) {
        size_t remainder = fbLen % 1024;
        clientTCP.write(fbBuf, remainder);
      }
    }
    clientTCP.print(tail);
    esp_camera_fb_return(fb);

    long startTimer = millis();
    boolean state = false;

    while ((startTimer + waitTime) > millis()) {
      Serial.print(".");
      delay(100);
      while (clientTCP.available()) {
        char c = clientTCP.read();
        if (state == true) getBody += String(c);
        if (c == '\n') {
          if (getAll.length() == 0) state = true;
          getAll = "";
        }
        else if (c != '\r')
          getAll += String(c);
        startTimer = millis();
      }
      if (getBody.length() > 0) break;
    }
    clientTCP.stop();
    // free(fbBuf);
    Serial.println(getBody);
  }
  else {
    getBody = "Connected to api.telegram.org failed.";
    Serial.println("Connected to api.telegram.org failed.");
  }
  
  return getBody;
}


void handleNewMessages(int numNewMessages) {
  // Получает сообщение и выполняет функцию роутера
  Serial.print("Handle New Messages: ");
  Serial.println(numNewMessages);

  for (int i = 0; i < numNewMessages; i++) {
    // Chat id of the requester
    String chat_id = String(bot.messages[i].chat_id);
    if (chat_id != chatId) {
      bot.sendMessage(chat_id, "Unauthorized user", "");
      continue;
    }

    String text = bot.messages[i].text;
    Serial.println(text);

    if (text == "/photo") {
      sendPhoto = true;
      Serial.println("New photo  request");
    }
  }
}

void get_updates_telegram() {
  // Получает обновления с телеграмма и отправляет их в хандлер
  int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
  while (numNewMessages) {
    Serial.println("got response");
    handleNewMessages(numNewMessages);
    numNewMessages = bot.getUpdates(bot.last_message_received + 1);
  }
  
}