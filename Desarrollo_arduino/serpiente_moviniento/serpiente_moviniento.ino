#include <SPI.h>
#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite canvas = TFT_eSprite(&tft);

// --- TUS COLORES ---
uint16_t a = tft.color565(234, 82, 255);      // Rosa/Morado principal
uint16_t b = tft.color565(240, 199, 255);     // Rosa claro       // Fondo Gris oscuro
uint16_t colorCuerpo = tft.color565(200, 50, 200);
uint16_t apple = tft.color565(228, 37, 65);

// --- VARIABLES DEL JUEGO ---
int posX[100], posY[100]; 
int tamano = 3; 
char direccion = 'D'; 
int veloci = 150;
int puntos = 0;
int manzanaX = 240, manzanaY = 160;

// Control de pantallas (0: Menú, 1: Jugando, 2: Game Over)
int estadoJuego = 0; 

// --- TU FUNCIÓN DE TEXTO ---
void Texto(int a, int b, uint16_t c, int d, String e) {
  tft.setCursor(a, b);
  tft.setTextColor(c, TFT_BLACK);
  tft.setTextSize(d);
  tft.print(e);
}

// --- FUNCIONES MODULARES ---

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
  while(true) {
    if(Serial.available() > 0) {
      Serial.read(); // Limpiar el buffer
      estadoJuego = 1; // Cambiar a estado Jugando
      resetJuego();
      tft.fillScreen(TFT_BLACK); // Limpiar pantalla para el juego
      break;
    }
    Texto(85, 35, a, 12, "BIT-BOA");
    Texto(140, 240, a, 2, "Valentina Garcia");
    Texto(140, 265, a, 2, "  Mateo Amaya  ");
    Texto(140, 290, a, 2, " Samuel Quesada ");
    Texto(85, 167, b, 2, "Oprime Start para iniciar");
    delay(100); 
  }
}

void mostrarGameOver() {
  tft.fillScreen(TFT_BLACK);
  while(true) {
    Texto(45, 100, a, 10, "GAME OVER");
    Texto(120, 220, b, 2, "Puntos: " + String(puntos));
    Texto(80, 260, a, 2, "Presiona tecla para reiniciar");
    delay(250);
    Texto(45, 100, TFT_BLACK, 10, "GAME OVER");

    
    if(Serial.available() > 0) {
      Serial.read();
      estadoJuego = 0; // Volver al menú
      break;
    }
    delay(200);
  }
}

void leerControl() {
  if (Serial.available() > 0) {
    char lectura = Serial.read();
    if (lectura == 'W' && direccion != 'S') direccion = 'W';
    if (lectura == 'S' && direccion != 'W') direccion = 'S';
    if (lectura == 'A' && direccion != 'D') direccion = 'A';
    if (lectura == 'D' && direccion != 'A') direccion = 'D';
  }
}

void actualizarLogica() {
  // Movimiento del cuerpo
  for (int i = tamano - 1; i > 0; i--) {
    posX[i] = posX[i - 1];
    posY[i] = posY[i - 1];
  }
  // Movimiento cabeza
  if (direccion == 'W') posY[0] -= 20;
  if (direccion == 'S') posY[0] += 20;
  if (direccion == 'A') posX[0] -= 20;
  if (direccion == 'D') posX[0] += 20;
// LOGICA DE LA MANZANA
 if (posX[0] == manzanaX && posY[0] == manzanaY) {
    tamano++;
    puntos += 100;
    if(veloci > 30) veloci -= 10;

    // --- NUEVO FILTRO DE POSICIÓN ---
    // Usamos un bucle para asegurarnos de que la nueva manzana sea válida
    bool manzanaEncimaCuerpo;
    do {
      manzanaEncimaCuerpo = false;
      // Generamos una posición aleatoria alineada (rejilla de 20px)
      manzanaX = random(1, 23) * 20; 
      manzanaY = random(1, 15) * 20;

      // Verificamos que la nueva manzana NO aparezca sobre el cuerpo
      for (int i = 0; i < tamano; i++) {
        if (manzanaX == posX[i] && manzanaY == posY[i]) {
          manzanaEncimaCuerpo = true;
          break; // Salimos del for si hay coincidencia
        }
      }
    } while (manzanaEncimaCuerpo);
    // --------------------------------------------
  }
  // Colisión Cuerpo
  for (int i = 1; i < tamano; i++) {
    if (posX[0] == posX[i] && posY[0] == posY[i]) {
      estadoJuego = 2;
    }
  }

  // Límites
  if (posX[0] < 0 || posX[0] >= 480 || posY[0] < 0 || posY[0] >= 320) {
    estadoJuego = 2;
  }
}

void dibujarJuego() {
  canvas.fillSprite(TFT_BLACK); // Limpiar el lienzo
  
  // 1. DIBUJAR PUNTUACIÓN (Arriba en el medio)
  canvas.setTextColor(b); // Usamos tu color rosa claro
  canvas.setTextSize(2);
  canvas.setTextDatum(MC_DATUM); // Centra el texto respecto al punto X dado
  canvas.drawString("SCORE: " + String(puntos), 240, 20); // 240 es la mitad de 480

  // 2. DIBUJAR MANZANA
  canvas.fillSmoothCircle(manzanaX + 10, manzanaY + 10, 8, apple);

  // 3. DIBUJAR SERPIENTE
  for (int i = 0; i < tamano; i++) {
    uint16_t c = (i == 0) ? a : colorCuerpo;
    canvas.fillRect(posX[i] + 1, posY[i] + 1, 18, 18, c); 
  }
  
  // 4. ENVIAR A PANTALLA
  canvas.pushSprite(0, 0);
}
// --- SETUP ---
void setup() {
  tft.init();
  tft.setRotation(1);
  canvas.createSprite(480, 320);
  Serial.begin(115200);
  randomSeed(analogRead(0)); // Para que las manzanas sean aleatorias de verdad
}

// --- LOOP PRINCIPAL ---
void loop() {
  if (estadoJuego == 0) {
    mostrarMenu();
  } 
  else if (estadoJuego == 1) {
    leerControl();
    actualizarLogica();
    dibujarJuego();
    delay(veloci);
  } 
  else if (estadoJuego == 2) {
    mostrarGameOver();
  }
}