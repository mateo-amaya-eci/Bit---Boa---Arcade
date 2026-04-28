// --- Definición de Pines según nuestra tabla ---
const int PIN_X = 4;  // Eje X (A0)
const int PIN_Y = 5;  // Eje Y (A1)

const int BTN_A = 6;  // Botón A (D2)
const int BTN_B = 7;  // Botón B (D3)
const int BTN_C = 15; // Botón C (D4)
const int BTN_D = 16; // Botón D (D5)
const int BTN_E = 17; // Botón E (D6)
const int BTN_F = 18; // Botón F (D7)
const int BTN_K = 8;  // Botón Joystick (D8)

void setup() {
  Serial.begin(115200);
  
  // Configuración de botones (INPUT_PULLUP porque el shield cierra a GND)
  pinMode(BTN_A, INPUT_PULLUP);
  pinMode(BTN_B, INPUT_PULLUP);
  pinMode(BTN_C, INPUT_PULLUP);
  pinMode(BTN_D, INPUT_PULLUP);
  pinMode(BTN_E, INPUT_PULLUP);
  pinMode(BTN_F, INPUT_PULLUP);
  pinMode(BTN_K, INPUT_PULLUP);

  Serial.println("--- Prueba de Joystick Shield con ESP32-S3 ---");
}

void loop() {
  // 1. Leer valores analógicos del Joystick (0 - 4095)
  int xVal = analogRead(PIN_X);
  int yVal = analogRead(PIN_Y);

  // 2. Leer estado de los botones (LOW = Presionado, HIGH = Suelto)
  bool aPresionado = digitalRead(BTN_A) == LOW;
  bool bPresionado = digitalRead(BTN_B) == LOW;
  bool cPresionado = digitalRead(BTN_C) == LOW;
  bool dPresionado = digitalRead(BTN_D) == LOW;
  bool ePresionado = digitalRead(BTN_E) == LOW;
  bool fPresionado = digitalRead(BTN_F) == LOW;
  bool kPresionado = digitalRead(BTN_K) == LOW;

  // 3. Mostrar resultados en el Monitor Serie
  if(xVal<824 && xVal>550){ 
    Serial.print("Derecha");
  }
  else if(xVal==0){ 
    Serial.print("Izquierda");
  }
  else if(yVal<824 && yVal>550){ 
    Serial.print("Arriba");
  }
  else if(yVal==0){ 
    Serial.print("Abajo");
  }
  Serial.println(); // Nueva línea

  delay(100); // Pequeña pausa para que sea legible
}