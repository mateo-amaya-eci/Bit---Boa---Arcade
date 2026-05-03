#include <esp_now.h>
#include <WiFi.h>

// --- CONFIGURACIÓN ESP-NOW ---
uint8_t broadcastAddress[] = {0xE0, 0x72, 0xA1, 0xD7, 0xF5, 0x00};

typedef struct struct_message {
  char a[32];
  int b;
} struct_message;

struct_message myData;

// --- CONFIGURACIÓN PINES DEL CONTROL ---
const int pinX = 1; 
const int pinY = 2;
const int pins[] = {3, 4, 5, 6, 7, 8, 9}; // btnA hasta btnK
char labels[] = {'W', 'D', 'S', 'A', 'V', 'M', 'K'};

int lastX = 0;
int lastY = 0;
bool lastBtnState[7] = {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH};

// Función para enviar el mensaje (el niño lanza la nota)
void enviarComando(char comando) {
  sprintf(myData.a, "%c", comando); // Ponemos la letra en el mensaje
  myData.b = millis() / 1000;
  esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
  
  Serial.print("Enviando comando: ");
  Serial.println(comando);
}

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  // Opcional: ver si llegó el mensaje
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  // Inicializar ESP-NOW
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

  // Configurar pines de botones
  for (int i = 0; i < 7; i++) {
    pinMode(pins[i], INPUT_PULLUP);
  }
}

void loop() {
  // 1. REVISAR BOTONES
  for (int i = 0; i < 7; i++) {
    bool currentState = digitalRead(pins[i]);
    
    // Si se presiona el botón (pasa de HIGH a LOW)
    if (currentState == LOW && lastBtnState[i] == HIGH) {
      enviarComando(labels[i]);
      delay(50); // Debounce
    }
    lastBtnState[i] = currentState;
  }

  // 2. REVISAR JOYSTICK
  int currentX = analogRead(pinX);
  int currentY = analogRead(pinY);

  // Sensibilidad del joystick
  if (abs(currentX - lastX) > 400 || abs(currentY - lastY) > 400) {
    if (currentX < 500) enviarComando('A');
    else if (currentX > 3500) enviarComando('D');
    
    if (currentY < 500) enviarComando('S');
    else if (currentY > 3500) enviarComando('W');
    
    lastX = currentX;
    lastY = currentY;
  }

  delay(10); 
}