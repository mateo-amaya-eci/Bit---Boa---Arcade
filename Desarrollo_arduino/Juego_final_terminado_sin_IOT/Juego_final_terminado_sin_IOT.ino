#include <TFT_eSPI.h>
#include <esp_now.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>

// Configuración de Red
#define WIFI_SSID "ingmateo"
#define WIFI_PASSWORD "mateohijo"

// Configuración de Firebase
#define API_KEY "TU_API_KEY_AQUI"
#define DATABASE_URL "https://bit---boa-default-rtdb.firebaseio.com/" 

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

bool datosEnviados = false;
// --- ESTRUCTURA ESP-NOW ---
typedef struct struct_message {
  char a[32];
  int b;
} struct_message;

struct_message incomingReadings;
char comando = ' '; 

// --- CONFIGURACIÓN PANTALLA ---
TFT_eSPI tft = TFT_eSPI();
TFT_eSprite canvas = TFT_eSprite(&tft);

// --- COLORES ---
uint16_t colA = tft.color565(234, 82, 255);      
uint16_t colB = tft.color565(240, 199, 255);
uint16_t colC = tft.color565(255, 92, 242);
uint16_t colD = tft.color565(255, 255, 255);        
uint16_t colorCuerpo = tft.color565(200, 50, 200);
uint16_t apple = tft.color565(228, 37, 65);

// --- VARIABLES DE ESTADO Y USUARIO ---
int estadoJuego = 0; // 0: Menu, 1: Seleccion, 2: Jugando, 3: Game Over
int LetraIdx = 0;
int posNombre = 0;
const char* Letras[] = {"A","B","C","D","E","F","G","H","I","J","K","L","M","N","O","P","Q","R","S","T","U","V","W","X","Y","Z"};
char us[5] = "----";

// --- VARIABLES DEL JUEGO (SNAKE) ---
int posX[100], posY[100]; 
int tamano = 3; 
char direccion = 'D'; 
int veloci = 151;
int puntos = 0;
int manzanaX, manzanaY;
int NNN=3;

// --- FUNCIÓN RECIBIR (ESP-NOW) ---
void OnDataRecv(const esp_now_recv_info *info, const uint8_t *incomingData, int len) {
  memcpy(&incomingReadings, incomingData, sizeof(incomingReadings));
  comando = incomingReadings.a[0]; 
}

void resetJuego() {
  tamano = 3;
  direccion = 'D';
  veloci = 151;
  puntos = 0;
  for(int i = 0; i < tamano; i++) {
    posX[i] = 100 - (i * 20);
    posY[i] = 100;
  }
  manzanaX = random(1, 23) * 20; 
  manzanaY = random(1, 15) * 20;
}

// --- PANTALLAS ---

void mostrarMenu() {
  canvas.fillSprite(TFT_BLACK);
  canvas.setTextDatum(MC_DATUM);
  canvas.setTextColor(colA);
  canvas.setTextSize(12);
  canvas.setTextColor(colA);
  canvas.drawString("BIT-BOA",240,35,1);
  canvas.setTextSize(2);
  canvas.drawString("Valentina Garcia", 240, 240, 1);
  canvas.drawString("Mateo Amaya",240,265,1);
  canvas.drawString("Samuel Quesada",240,290,1);
  canvas.setTextColor(colB);
  canvas.drawString("Presiona START para iniciar",240,167,1);
  canvas.pushSprite(0, 0);
  
  
  while(estadoJuego == 0) {
    if(comando == 'V') { 
      estadoJuego = 1; 
      comando = ' ';
    }
    delay(10);
  }
}

void dibujarFlechas(int resalte) {
  uint16_t cArriba = (resalte == 1) ? colD : colA;
  canvas.fillTriangle(240, 80, 200, 110, 280, 110, cArriba);
  uint16_t cAbajo = (resalte == 2) ? colD : colA;
  canvas.fillTriangle(240, 240, 200, 210, 280, 210, cAbajo);
}

void dibujarSeleccion(int flecha) {
  canvas.fillSprite(TFT_BLACK);
  canvas.setTextDatum(MC_DATUM);
  canvas.setTextColor(colB);
  canvas.setTextSize(2);
  canvas.drawString("SELECCIONA TU USUARIO", 240, 40, 1);
  canvas.setTextColor(colA);
  canvas.setTextSize(10); 
  canvas.drawString(Letras[LetraIdx], 245, 160, 1); 
  dibujarFlechas(flecha);
  canvas.setTextSize(5); 
  canvas.setTextColor(colB);
  canvas.drawString(us, 380, 250, 1);
  canvas.setTextSize(3); 
  canvas.drawString("BIT-BOA", 100, 250, 1);    
  canvas.setTextSize(1);
  canvas.setTextSize(2); 
  canvas.setTextColor(colB);
  canvas.drawString("SELECT para fijar letra", 240, 300, 1);
  canvas.pushSprite(0, 0);
}

void seccionUsuario() {
  posNombre = 0;
  strcpy(us, "----");
  dibujarSeleccion(0);
  
  while(estadoJuego == 1) {
    if (comando != ' ') {
      if (posNombre < 4) {
        if (comando == 'W') { LetraIdx = (LetraIdx + 1) % 26; dibujarSeleccion(1); delay(150); }
        else if (comando == 'S') { LetraIdx = (LetraIdx - 1 < 0) ? 25 : LetraIdx - 1; dibujarSeleccion(2); delay(150); }
        else if (comando == 'M') {
          us[posNombre] = Letras[LetraIdx][0];
          posNombre++;
          canvas.fillCircle(240, 160, 60, colB); canvas.pushSprite(0,0); delay(200);
          dibujarSeleccion(0);
        }
      } else {
        NNN=3;
        if (NNN>0) {
          while(NNN>0){
            canvas.fillSprite(TFT_BLACK);
            canvas.setTextDatum(MC_DATUM);
            canvas.setTextColor(colA);
            canvas.setTextSize(2);
            canvas.drawString("ESTAS PREPARADO JUGADOR ", 220, 130, 1);
            canvas.setTextColor(colB);
            canvas.setTextSize(2);
            canvas.drawString(String(us), 390, 130, 1);
            canvas.setTextColor(colA);
            canvas.setTextSize(2);
            canvas.drawString("TU PARIDA EMPIEZA EN:", 240, 170, 1);
            canvas.setTextColor(colA);
            canvas.setTextSize(8);
            canvas.drawString(String(NNN), 240, 240, 1);
            canvas.pushSprite(0,0);
            NNN-=1;
            delay(1000);
          }
          estadoJuego = 2; 
          resetJuego(); 
        }
      }
      comando = ' ';
    }
    delay(10);
  }
}

void actualizarLogicaSnake() {
  // Solo actualiza dirección si no es contraria
  if (comando == 'W' && direccion != 'S') direccion = 'W';
  if (comando == 'S' && direccion != 'W') direccion = 'S';
  if (comando == 'A' && direccion != 'D') direccion = 'A';
  if (comando == 'D' && direccion != 'A') direccion = 'D';
  comando = ' ';

  for (int i = tamano - 1; i > 0; i--) { posX[i] = posX[i - 1]; posY[i] = posY[i - 1]; }
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
  for (int i = 1; i < tamano; i++) { if (posX[0] == posX[i] && posY[0] == posY[i]) estadoJuego = 3; }
  if (posX[0] < 0 || posX[0] >= 480 || posY[0] < 0 || posY[0] >= 320) estadoJuego = 3;
}

void dibujarSnake() {
  canvas.fillSprite(TFT_BLACK);
  canvas.setTextColor(colB);
  canvas.setTextDatum(TL_DATUM);
  canvas.setTextSize(2);
  canvas.drawString(String(us) + " SCORE: " + String(puntos), 160, 10, 1);
  canvas.fillSmoothCircle(manzanaX + 10, manzanaY + 10, 8, apple);
  for (int i = 0; i < tamano; i++) {
    uint16_t c = (i == 0) ? colA : colorCuerpo;
    canvas.fillRect(posX[i] + 1, posY[i] + 1, 18, 18, c); 
  }
  canvas.pushSprite(0, 0);
}

void mostrarGameOver() {
  unsigned long tiempoAnterior = 0;
  bool visible = false;
  const long intervalo = 400;

  while(estadoJuego == 3) {
    unsigned long tiempoActual = millis();

    if (tiempoActual - tiempoAnterior >= intervalo) {
      tiempoAnterior = tiempoActual;
      visible = !visible;
    }

    canvas.fillSprite(TFT_BLACK);
    
    canvas.setTextDatum(TL_DATUM); // Alineación a la izquierda para las instrucciones
    canvas.setTextSize(2);
    canvas.setTextColor(colB);
    canvas.drawString("START: Jugar otra vez", 80, 240, 1);
    canvas.setTextColor(colC); // Usando colA para resaltar SELECT
    canvas.drawString("SELECT: Volver al inicio", 80, 270, 1);


    canvas.setTextDatum(MC_DATUM); // Alineación centrada para el puntaje
    canvas.setTextSize(4);
    canvas.setTextColor(colA);
    canvas.drawString(String(us) + ": " + String(puntos), 240, 180, 1);

    if (visible) {
      canvas.setTextSize(10);
      canvas.setTextColor(colA);
      canvas.drawString("GAME OVER", 240, 80, 1);
    }

    canvas.pushSprite(0, 0);

    if (comando == 'V') { 
      estadoJuego = 2; 
      resetJuego(); 
      comando = ' '; 
    }
    if (comando == 'M') { 
      estadoJuego = 0; 
      comando = ' '; 
    }
    
    delay(10); // Estabilidad para el procesador
  }
}

// --- SETUP ---
void setup() {
  tft.init();
  tft.setRotation(1);
  canvas.createSprite(480, 320);
  WiFi.mode(WIFI_STA);
  esp_now_init();
  esp_now_register_recv_cb(OnDataRecv);
  randomSeed(analogRead(0));
}

// --- LOOP ---
void loop() {
  if (estadoJuego == 0) mostrarMenu();
  else if (estadoJuego == 1) seccionUsuario();
  else if (estadoJuego == 2) {
    actualizarLogicaSnake();
    dibujarSnake();
    delay(veloci);
  } 
  else if (estadoJuego == 3) mostrarGameOver();
}