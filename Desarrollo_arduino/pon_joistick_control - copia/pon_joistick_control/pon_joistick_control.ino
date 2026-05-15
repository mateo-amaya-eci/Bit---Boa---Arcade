const int pinX = 1; 
const int pinY = 2;
const int btnA = 3;
const int btnB = 4;
const int btnC = 5;
const int btnD = 6;
const int btnE = 7;
const int btnF = 8;
const int btnK = 9; 


int lastX = 0;
int lastY = 0;
bool lastBtnState[7] = {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH};
int pins[] = {btnA, btnB, btnC, btnD, btnE, btnF, btnK};
char labels[] = {'A', 'B', 'C', 'D', 'E', 'F', 'K'};

void setup() {
  Serial.begin(115200);
  
 
  for (int i = 0; i < 7; i++) {
    pinMode(pins[i], INPUT_PULLUP);
  }
  
  Serial.println("Sistema listo. Presiona un boton o mueve el joystick...");
}

void loop() {

  for (int i = 0; i < 7; i++) {
    bool currentState = digitalRead(pins[i]);
    
   
    if (currentState == LOW && lastBtnState[i] == HIGH) {
      Serial.printf("Boton %c presionado\n", labels[i]);
      delay(50); 
    }
    lastBtnState[i] = currentState;
  }

  
  int currentX = analogRead(pinX);
  int currentY = analogRead(pinY);

  
  if (abs(currentX - lastX) > 150 || abs(currentY - lastY) > 150) {
    if (currentX < 500) Serial.println("Joystick: IZQUIERDA");
    else if (currentX > 3500) Serial.println("Joystick: DERECHA");
    
    if (currentY < 500) Serial.println("Joystick: ABAJO");
    else if (currentY > 3500) Serial.println("Joystick: ARRIBA");
    
    lastX = currentX;
    lastY = currentY;
  }

  delay(10); 
}