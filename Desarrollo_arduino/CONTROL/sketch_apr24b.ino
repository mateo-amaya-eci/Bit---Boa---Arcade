#include <esp_now.h>
#include <WiFi.h>

typedef struct struct_message {
  char a[32];
  int b;
} struct_message;

struct_message incomingReadings;

// Callback al recibir
void OnDataRecv(const esp_now_recv_info *info, const uint8_t *incomingData, int len) {
  memcpy(&incomingReadings, incomingData, sizeof(incomingReadings));





  Serial.println(incomingReadings.a);

}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error inicializando ESP-NOW");
    return;
  }
  
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {}