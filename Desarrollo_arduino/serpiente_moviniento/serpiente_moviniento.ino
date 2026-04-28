#include <SPI.h>
#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite canvas = TFT_eSprite(&tft);

// Variables de la serpiente
int posX[100]; // Arreglo para guardar posiciones X de cada cuadro
int posY[100]; // Arreglo para guardar posiciones Y de cada cuadro
int tamano = 3; // Tamaño inicial
char direccion = 'D'; // R: Derecha, L: Izquierda, U: Arriba, D: Abajo

// Variables de la manzana
int manzanaX = 240, manzanaY = 160;

// Colores
uint16_t colorCabeza = tft.color565(234, 82, 255);
uint16_t colorCuerpo = tft.color565(200, 50, 200);
uint16_t colorManzana = tft.color565(228, 37, 65);
uint16_t fondo = tft.color565(48, 48, 48);

void setup() {
  tft.init();
  tft.setRotation(1);
  canvas.createSprite(480, 320);
  Serial.begin(115200);

  // Inicializar posiciones iniciales de la serpiente
  for(int i = 0; i < tamano; i++) {
    posX[i] = 180 - (i * 20);
    posY[i] = 150;
  }
}

void loop() {
  // 1. Leer Serial para cambiar dirección (Enviado por tu otra ESP32)
  if (Serial.available() > 0) {
    char lectura = Serial.read();
    // Evitar que gire 180 grados sobre sí misma
    if (lectura == 'W' && direccion != 'S') direccion = 'W';
    if (lectura == 'S' && direccion != 'W') direccion = 'S';
    if (lectura == 'A' && direccion != 'D') direccion = 'A';
    if (lectura == 'D' && direccion != 'A') direccion = 'D';
  }

  // 2. Lógica de Movimiento (Intercambio de posiciones en el vector)
  for (int i = tamano - 1; i > 0; i--) {
    posX[i] = posX[i - 1];
    posY[i] = posY[i - 1];
  }

  // 3. Mover la cabeza
  if (direccion == 'W') posY[0] -= 20;
  if (direccion == 'S') posY[0] += 20;
  if (direccion == 'A') posX[0] -= 20;
  if (direccion == 'D') posX[0] += 20;

  // 4. Lógica de la Manzana (Detección de colisión)
  // Si la cabeza está cerca de la manzana (margen de 20px)
  if (abs(posX[0] - manzanaX) < 20 && abs(posY[0] - manzanaY) < 20) {
    tamano++; // ¡Crece!
    manzanaX = (random(1, 23)) * 20; // Nueva posición aleatoria
    manzanaY = (random(1, 15)) * 20;
  }

  // 5. Dibujar en el Canvas
  canvas.fillSprite(fondo);
  
  // Dibujar manzana
  canvas.fillSmoothCircle(manzanaX, manzanaY, 8, colorManzana);

  // Dibujar cuerpo
  for (int i = 0; i < tamano; i++) {
    uint16_t c = (i == 0) ? colorCabeza : colorCuerpo;
    canvas.fillRect(posX[i], posY[i], 18, 18, c); // 18 para dejar un pequeño borde
  }

  canvas.pushSprite(0, 0);
  // 5. Comprobar colisión con el propio cuerpo
  // Empezamos desde i = 1 porque la posición 0 es la cabeza
  for (int i = 1; i < tamano; i++) {
    if (posX[0] == posX[i] && posY[0] == posY[i]) {
      // Si la cabeza está en el mismo lugar que cualquier parte del cuerpo...
      tft.fillScreen(fondo);
      tft.setTextColor(TFT_WHITE);
      tft.setTextSize(5);
      tft.setCursor(70, 140);
      tft.print("AUTO-CHOQUE!"); 
      delay(2000);
      
      // Reiniciar juego o bloquear
  
    }
  }

  // 6. Límites (Game Over)
  if (posX[0] < 0 || posX[0] > 460 || posY[0] < 0 || posY[0] > 300) {
     tft.fillScreen(fondo);
     tft.setTextColor(TFT_WHITE);
     tft.setTextSize(5);
     tft.setCursor(100, 140);
     tft.print("GAME OVER");
     while(1); // Bloquear juego
  }

  delay(150); // Velocidad del juego
}