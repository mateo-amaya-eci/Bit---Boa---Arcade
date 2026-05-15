#include <esp_now.h>
#include <WiFi.h>

uint8_t broadcastAddress[] = {0x44, 0x1B, 0xF6, 0x2D, 0xD4, 0x00}; // Aca colocar la direccion del MAC de la ESP32-S3, ( lo puedes buscar en el codigo, direccion MAC)

typedef struct struct_message {
  char a[32];
  int b;
} struct_message;

struct_message myData;

const int pinX = 1; 
const int pinY = 2;
const int pins[] = {3, 4, 5, 6, 7, 8, 9}; 
char labels[] = {'W', 'D', 'S', 'A', 'V', 'M', 'K'};

bool lastBtnState[7] = {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH};


char lastJoyCommand = ' '; 

void enviarComando(char comando) {
  if (comando == ' ') return; // No enviar nada si es vacío
  
  sprintf(myData.a, "%c", comando);
  myData.b = millis() / 1000;
  esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
  
  Serial.print("Enviando comando: ");
  Serial.println(comando);
}

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error inicializando ESP-NOW");
    return;
  }

  esp_now_register_send_cb((esp_now_send_cb_t)OnDataSent);
  
  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Error al agregar par");
    return;
  }

  for (int i = 0; i < 7; i++) {
    pinMode(pins[i], INPUT_PULLUP);
  }
}

void loop() {
  // --- 1. BOTONES (CON DEBOUNCE MEJORADO) ---
  for (int i = 0; i < 7; i++) {
    bool currentState = digitalRead(pins[i]);
    if (currentState == LOW && lastBtnState[i] == HIGH) {
      enviarComando(labels[i]);
      delay(150); // Aumentamos el delay de rebote para evitar ráfagas
    }
    lastBtnState[i] = currentState;
  }

  // --- 2. JOYSTICK (CON LÓGICA DE ESTADO) ---
  int currentX = analogRead(pinX);
  int currentY = analogRead(pinY);
  char currentJoyCommand = ' ';

  
  if (currentX < 1000) currentJoyCommand = 'A';
  else if (currentX > 3000) currentJoyCommand = 'D';
  else if (currentY < 1000) currentJoyCommand = 'S';
  else if (currentY > 3000) currentJoyCommand = 'W';

  // Solo envía si la dirección cambió (para no saturar el receptor)
  if (currentJoyCommand != lastJoyCommand) {
    if (currentJoyCommand != ' ') {
      enviarComando(currentJoyCommand);
    }
    lastJoyCommand = currentJoyCommand;
  }

  delay(20); 
}