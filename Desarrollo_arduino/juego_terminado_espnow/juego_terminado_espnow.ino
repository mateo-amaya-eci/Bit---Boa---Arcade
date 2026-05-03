#include <SPI.h>
#include <TFT_eSPI.h>
#include <esp_now.h>
#include <WiFi.h>

// --- ESTRUCTURA ESP-NOW ---
typedef struct struct_message {
  char a[32];
  int b;
} struct_message;

struct_message incomingReadings;

// --- CONFIGURACIÓN PANTALLA ---
TFT_eSPI tft = TFT_eSPI();
TFT_eSprite canvas = TFT_eSprite(&tft);

// --- TUS COLORES ---
uint16_t a = tft.color565(234, 82, 255);      
uint16_t b = tft.color565(240, 199, 255);     
uint16_t colorCuerpo = tft.color565(200, 50, 200);
uint16_t apple = tft.color565(228, 37, 65);

// --- VARIABLES DEL JUEGO ---
int posX[100], posY[100]; 
int tamano = 3; 
char direccion = 'D'; 
int veloci = 151;
int puntos = 0;
int manzanaX = 240, manzanaY = 160;
int estadoJuego = 0; 

// --- FUNCIÓN CALLBACK (Aquí llega el control inalámbrico) ---
void OnDataRecv(const esp_now_recv_info *info, const uint8_t *incomingData, int len) {
  memcpy(&incomingReadings, incomingData, sizeof(incomingReadings));
  
  char comando = incomingReadings.a[0]; 

  // --- LÓGICA DE INICIO CON LA LETRA 'V' ---
  if (comando == 'V') {
    if (estadoJuego == 0) { // Si está en el Menú
      estadoJuego = 1; 
      resetJuego();
    } 
    else if (estadoJuego == 2) { // Si está en Game Over
      estadoJuego = 0; // Regresa al menú primero (o pon 1 para reiniciar directo)
    }
  }

  // Control de dirección (esto solo funciona si ya estás jugando)
  if (estadoJuego == 1) {
    if (comando == 'W' && direccion != 'S') direccion = 'W';
    if (comando == 'S' && direccion != 'W') direccion = 'S';
    if (comando == 'A' && direccion != 'D') direccion = 'A';
    if (comando == 'D' && direccion != 'A') direccion = 'D';
  }
}

// --- FUNCIONES DEL JUEGO ---

void Texto(int x, int y, uint16_t color, int size, String msg) {
  tft.setCursor(x, y);
  tft.setTextColor(color, TFT_BLACK);
  tft.setTextSize(size);
  tft.print(msg);
}

void resetJuego() {
  tamano = 3;
  direccion = 'D';
  veloci = 150;
  puntos = 0;
  for(int i = 0; i < tamano; i++) {
    posX[i] = 100 - (i * 20);
    posY[i] = 100;
  }
  manzanaX = random(1, 23) * 20; 
  manzanaY = random(1, 15) * 20;
}

void mostrarMenu() {
  tft.fillScreen(TFT_BLACK);
  Texto(85, 35, a, 12, "BIT-BOA");
  Texto(140, 240, a, 2, "Valentina Garcia");
  Texto(140, 265, a, 2, " Mateo Amaya ");
  Texto(140, 290, a, 2, " Samuel Quesada ");
  Texto(80, 167, b, 2, "Usa el control para iniciar");
  
  while(estadoJuego == 0) { delay(10); } // Espera a que OnDataRecv cambie el estado
}

void mostrarGameOver() {
  tft.fillScreen(TFT_BLACK);
  Texto(45, 100, a, 10, "GAME OVER");
  Texto(120, 220, b, 2, "Puntos: " + String(puntos));
  Texto(60, 260, a, 2, "Presiona cualquier boton para menu");
  
  while(estadoJuego == 2) { delay(10); } // Espera a que OnDataRecv cambie el estado
}

void actualizarLogica() {
  for (int i = tamano - 1; i > 0; i--) {
    posX[i] = posX[i - 1];
    posY[i] = posY[i - 1];
  }
  if (direccion == 'W') posY[0] -= 20;
  if (direccion == 'S') posY[0] += 20;
  if (direccion == 'A') posX[0] -= 20;
  if (direccion == 'D') posX[0] += 20;

  if (posX[0] == manzanaX && posY[0] == manzanaY) {
    tamano++;
    puntos += 100;
    if(veloci > 1) veloci -= 10;

    bool manzanaEncimaCuerpo;
    do {
      manzanaEncimaCuerpo = false;
      manzanaX = random(1, 23) * 20; 
      manzanaY = random(1, 15) * 20;
      for (int i = 0; i < tamano; i++) {
        if (manzanaX == posX[i] && manzanaY == posY[i]) {
          manzanaEncimaCuerpo = true;
          break;
        }
      }
    } while (manzanaEncimaCuerpo);
  }

  for (int i = 1; i < tamano; i++) {
    if (posX[0] == posX[i] && posY[0] == posY[i]) estadoJuego = 2;
  }
  if (posX[0] < 0 || posX[0] >= 480 || posY[0] < 0 || posY[0] >= 320) estadoJuego = 2;
}

void dibujarJuego() {
  canvas.fillSprite(TFT_BLACK);
  canvas.setTextColor(b);
  canvas.setTextSize(2);
  canvas.setTextDatum(MC_DATUM);
  canvas.drawString("SCORE: " + String(puntos), 240, 20);
  canvas.fillSmoothCircle(manzanaX + 10, manzanaY + 10, 8, apple);
  for (int i = 0; i < tamano; i++) {
    uint16_t c = (i == 0) ? a : colorCuerpo;
    canvas.fillRect(posX[i] + 1, posY[i] + 1, 18, 18, c); 
  }
  canvas.pushSprite(0, 0);
}

// --- SETUP ---
void setup() {
  tft.init();
  tft.setRotation(1);
  canvas.createSprite(480, 320);
  Serial.begin(115200);
  
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error ESP-NOW");
    return;
  }
  esp_now_register_recv_cb(OnDataRecv);
  
  randomSeed(analogRead(0));
}

// --- LOOP PRINCIPAL ---
void loop() {
  if (estadoJuego == 0) mostrarMenu();
  else if (estadoJuego == 1) {
    actualizarLogica();
    dibujarJuego();
    delay(veloci);
  } 
  else if (estadoJuego == 2) mostrarGameOver();
}