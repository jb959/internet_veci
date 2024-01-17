// treba zapojit do 5V 
#include <DHT.h>
#define DHT_SENSOR_PIN 15
#define DHT_SENSOR_TYPE DHT11

DHT dht_sensor(DHT_SENSOR_PIN, DHT_SENSOR_TYPE);

#include <Wire.h>

#include <Adafruit_GFX.h>

#include <Adafruit_SSD1306.h>
 // treb  pripnut z displaya vyvody:
// ground  na uzemnenie, VCC je na 5V
// pozor treba spravne zapojit oled display aby spravne zobrazoval
// ground na opacnu stranu od 5V 
// SDA na pin cislo 21
// SCL na pin cislo 22
#define SCREEN_WIDTH 128 // OLED width,  in pixels
#define SCREEN_HEIGHT 64 // OLED height, in pixels

// create an OLED display object connected to I2C
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, & Wire, -1);

int wanted = 24;
// povodna  wanted teplota, podla nej budeme porovnavat (ci ma prostredie spravnu teplotu)
// mozne zmenit pomocov tlacidiel
// a mozne aj zmenit ci ma bzucat ked je  namerana teplota vecsia alebo mensia 

#include <ezButton.h>

// tlacidla  ez button, budu 3
#define plusPIN 25
#define minusPIN 27
#define otocPIN 32
// tlacidla pin , a druha strana ku GND ale iba to na opacnej strane 

ezButton plus(plusPIN);
ezButton minus(minusPIN);
ezButton otoc(otocPIN);

#define bzucPIN 12
// pin pre bzuciak je 12

bool chcemViac = true;
// ci chceme vecsiu teplotu
// ak false tak chceme mensiu teplotu
bool maBzucat = false;
//  ci ma bzucat

void zobraz(float teplota, int wanted) {

  oled.clearDisplay();
  oled.setTextSize(1.5);
  oled.setTextColor(WHITE);
  oled.setCursor(0, 10);
  // zaklad  zobrazenia

  oled.print(" ");
  oled.print("namerana teplota: \n ");
  oled.print(teplota);
  oled.print("°C  \n ");
  oled.print("ziadana teplota ");
  oled.print("\n");
  if (chcemViac == true) {
    oled.print(" > ");
  } else if (chcemViac == false) {
    oled.print(" < ");
  } 
  oled.print(wanted);
  oled.print("\n");
  // moznosti
  // prilis teple, prilis chladne, 
  // dostatocne teple, dostatocne chladne
  // ked je nieco primoc tak bude bzucat, ked je nieco dostatocne tak bude potichu   

  if (teplota < wanted) {
    oled.println("mensia nez ziadana");
    if (chcemViac == true) {
      oled.println("!!prilis zima!!!!");
      maBzucat = true;
    } else if (chcemViac == false) {
      oled.println("dostatocne chladno");
      maBzucat = false;
    }
  } else if (teplota > wanted) {
    oled.println("vacsia nez ziadana");
    if (chcemViac == true) {
      oled.println("dostatocne teplo");
      maBzucat = false;
    } else if (chcemViac == false) {
      oled.println("!!prilis horuco!!!!");
      maBzucat = true;

    }
  }

  oled.display();
}

void setup(){
  Serial.begin(9600);
  dht_sensor.begin();

  int pet = 50;
  plus.setDebounceTime(pet);
  minus.setDebounceTime(pet);
  otoc.setDebounceTime(pet);

  if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("failed to start SSD1306 OLED"));
    while (1);
  }

  pinMode(bzucPIN, OUTPUT); // pin pre bzuciak na output mode
  // bzuciak zapne a vypne pomocov digital write

}

void loop() {

  minus.loop();
  plus.loop();
  otoc.loop();

  if (plus.isPressed()) {
    wanted = wanted + 1;
    Serial.println(" stlacene plus, zvysene na  " + wanted);

  } else if (minus.isPressed()) {
    wanted = wanted - 1;
    Serial.println(" stlacene minus znizene na " + wanted);

  } else if (otoc.isPressed()) {
    chcemViac = !chcemViac;
    if (chcemViac == true) {
      Serial.println("CHCEM VECSIU TEPLOTU NEZ ZADANU " + wanted);
    } else if (chcemViac == false) {
      Serial.println("CHCEM MENSIU TEPLOTU NEZ ZADANU " + wanted);
    }
  } else {}

  float teplota = dht_sensor.readTemperature();
  // zmerat teplotu
  if (isnan(teplota)) {
    // chybove hlasenie 
    Serial.println("nemozne precitat teplotu zo senzoru!");
  } else {

    if (maBzucat == true) {
      digitalWrite(bzucPIN, HIGH);
    } else if (maBzucat == false) {
      digitalWrite(bzucPIN, LOW);
    }

    zobraz(teplota, wanted);

    //delay(2000);
    // delay dat prec inak nepojdu dobre tlacidla
    // moze v tomto byt multithreading?
  }
}