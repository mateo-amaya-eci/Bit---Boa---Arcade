#include <SPI.h>
#include <TFT_eSPI.h>
//#include <string.h>
TFT_eSPI tft = TFT_eSPI();
TFT_eSprite canvas = TFT_eSprite(&tft);

uint16_t a=tft.color565(234,82,255);
uint16_t b=tft.color565(240,199,255);
uint16_t l=tft.color565(48,48,48);

void Texto(int a,int b,uint16_t c,int d, String e){ // x , y , color , tamaño , texto
  tft.setCursor(a,b);
  tft.setTextColor(c,l);
  tft.setTextSize(d);
  tft.print(e);
}

void setup(void) {
  tft.init();
  tft.setRotation(1);
  Serial.begin(9600);
  
}

void loop() {
  tft.fillScreen(l);
  while(true){ 
    if(Serial.available()>0){
      break;
    }
    Texto(85,35,a,12, "BIT-BOA");// x , y , color , tamaño , texto
    Texto(140,240,a,2,"Valentina Garcia");
    Texto(140,265,a,2,"  Mateo Amaya  ");
    Texto(140,290,a,2," Samuel Quesada ");
    Texto(85,167,b,2,"Oprime Start para iniciar");     
  }

  Texto(45,130,a,10,"GAME OVER");
  delay(200);
  
}



