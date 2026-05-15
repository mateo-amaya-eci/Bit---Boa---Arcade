#include "WiFi.h"
#include "esp_wifi.h" // Librería nativa para forzar el inicio

void setup() {
  Serial.begin(115200);
  
  // Tiempo para que el monitor serial conecte
  delay(2000); 
  Serial.println("\n--- TEST DE MAC ---");

  // Forzamos el encendido de la radio WiFi
  WiFi.mode(WIFI_STA);
  
  // Pequeño bucle de espera por si la radio tarda en responder
  int intentos = 0;
  while (WiFi.macAddress() == "00:00:00:00:00:00" && intentos < 10) {
    delay(500);
    Serial.print(".");
    intentos++;
  }

  Serial.println("");
  Serial.print("Dirección MAC Real: ");
  Serial.println(WiFi.macAddress());
}

void loop() {}