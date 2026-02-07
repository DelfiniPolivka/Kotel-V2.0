//////////////////////////////////////////////////////////////// LCD display
#include <LiquidCrystal.h>
const int rs = 12, en = 11, d4 = 7, d5 = 6, d6 = 5, d7 = 4;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

byte stupen[8] = {
  B00111,
  B00101,
  B00111,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000
};
//////////////////////////////////////////////////////////////// Nextion
String buffer1 = "";

//////////////////////////////////////////////////////////////// Teplotní čidla
static unsigned long cas_nacteni_teplomeru = 0;
static unsigned long cas_svitu = 0;

#include <OneWire.h>
#include <DallasTemperature.h>

OneWire oneWire_kotel(2);
OneWire oneWire_sud(3);

DallasTemperature kotel(&oneWire_kotel);
DallasTemperature sud(&oneWire_sud);
byte pocet_prav = 5;

//////////////////////////////////////////////////////////////// Přímání teploty z koupelny, kuchyne a pokoje
//////////////////////////////// Kuchyň
float teplomer_kuchyn = NAN;

String buffer2 = "";

//////////////////////////////// Koupelna a pokoj
float teplomer_koupelna = NAN;
float teplomer_pokoj = NAN;

String buffer3 = "";
//////////////////////////////////////////////////////////////// Přímání teploty nextion přednastavení
float prednastavena_pokoj = 22;
float prednastavena_kuchyn = 22;
float prednastavena_koupelna = 22;

//////////////////////////////////////////////////////////////// Proměnné pro hlídání teploty
//////////////////////////////// Kotel
float opravdu_stavNouze_kotel = 100;
float stavNouze_kotel = 90;
float teplota_kotle = 0;

static unsigned long cas_sireny = 0;

bool predchozi_stav = true;

bool predchozi_stav_podlaha_topeni = false;
bool predchozi_stav_dilna_radiatory = false;
bool predchozi_stav_tlacitko_auto_manual = true;

//////////////////////////////// Kohouty
bool stav_kohout_koupelna = true;
bool stav_kohout_kuchyn = true;
bool stav_kohout_pokoj = true;
bool stav_tlacitko_auto_manual = true;

enum Mistnosti {
  KOUPELNA,
  KUCHYN,
  POKOJ
};

//////////////////////////////////////////////////////////////// Tlačítka LED
//////////////////////////////// Piny tlačítek
const int pin_tlacitko_ventilator = 22;
const int pin_tlacitko_dmychadlo = 23;
const int pin_tlacitko_obeh_TUV = 24;
const int pin_tlacitko_podlaha_topeni = 25;
const int pin_tlacitko_dilna_radiatory = 26;

//////////////////////////////// Piny výstupů (LED / relé)
const int pin_led_ventilator = 27;
const int pin_led_dmychadlo = 28;
const int pin_led_obeh_TUV = 29;
const int pin_led_podlaha_topeni = 30;
const int pin_led_dilna_radiatory = 31;

//////////////////////////////// Stavy výstupů
bool stav_ventilator = false;
bool stav_dmychadlo = false;
bool stav_obeh_TUV = false;
bool stav_podlaha_topeni = false;
bool stav_dilna_radiatory = false;

//////////////////////////////// Přepínání tlačítek
bool ventilator_opakovani = false;
bool dmychadlo_opakovani = false;
bool obeh_TUV_opakovani = false;
bool podlaha_topeni_opakovani = false;
bool dilna_radiatory_opakovani = false;

//////////////////////////////// Předchozí stavy tlačítek
bool ventilator_predchozi = false;
bool dmychadlo_predchozi = false;
bool obeh_TUV_predchozi = false;
bool podlaha_topeni_predchozi = false;
bool dilna_radiatory_predchozi = false;

//////////////////////////////////////////////////////////////// Relé

const int rele1 = 53;  //větrák
const int rele2 = 52;  //dmychadlo
const int rele3 = 51;  //čerpadlo TUV
const int rele4 = 50;  //čerpadlo podlaha
const int rele5 = 49;  //kohout dílna radiatory
const int rele6 = 48;  //kohout koupelna
const int rele7 = 47;  //kohout kuchyň
const int rele8 = 46;  //kohout neobsazen

const int rel2e1 = 45;  //kohout pokoj
const int rel2e2 = 44;
const int rel2e3 = 43;
const int rel2e4 = 42;
const int rel2e5 = 41;
const int rel2e6 = 40;
const int rel2e7 = 39;
const int rel2e8 = 38;  //siréna

//////////////////////////////////////////////////////////////// Přechod mezi stránkami
bool stranka_ventilator = false;
bool stranka_obeh_TUV = false;
bool stranka_podlaha_topeni = false;
bool stranka_dilna_radiatory = false;
bool stranka_kohout_koupelna = false;
bool stranka_kohout_kuchyn = false;
bool stranka_kohout_pokoj = false;

void setup() {
  Serial.begin(9600);

  //////////////////////////////////////////////////////////////// Teloměry
  Serial1.begin(9600);  //Display
  Serial2.begin(9600);  //Kuchyň
  Serial3.begin(9600);  //Koupelna a pokoj
  kotel.begin();
  sud.begin();

  //////////////////////////////////////////////////////////////// LCD display
  lcd.begin(16, 2);
  lcd.createChar(0, stupen);

  //////////////////////////////////////////////////////////////// Tlačítka
  pinMode(pin_tlacitko_ventilator, INPUT_PULLUP);
  pinMode(pin_tlacitko_dmychadlo, INPUT_PULLUP);
  pinMode(pin_tlacitko_obeh_TUV, INPUT_PULLUP);
  pinMode(pin_tlacitko_podlaha_topeni, INPUT_PULLUP);
  pinMode(pin_tlacitko_dilna_radiatory, INPUT_PULLUP);

  //////////////////////////////// Tlačítka LED
  pinMode(pin_led_ventilator, OUTPUT);
  pinMode(pin_led_dmychadlo, OUTPUT);
  pinMode(pin_led_obeh_TUV, OUTPUT);
  pinMode(pin_led_podlaha_topeni, OUTPUT);
  pinMode(pin_led_dilna_radiatory, OUTPUT);

  //////////////////////////////////////////////////////////////// Relé
  pinMode(rele1, OUTPUT);
  pinMode(rele2, OUTPUT);
  pinMode(rele3, OUTPUT);
  pinMode(rele4, OUTPUT);
  pinMode(rele5, OUTPUT);
  pinMode(rele6, OUTPUT);
  pinMode(rele7, OUTPUT);
  pinMode(rele8, OUTPUT);
  pinMode(rel2e1, OUTPUT);
  pinMode(rel2e2, OUTPUT);
  pinMode(rel2e3, OUTPUT);
  pinMode(rel2e4, OUTPUT);
  pinMode(rel2e5, OUTPUT);
  pinMode(rel2e6, OUTPUT);
  pinMode(rel2e7, OUTPUT);
  pinMode(rel2e8, OUTPUT);

  digitalWrite(rele1, HIGH);
  digitalWrite(rele2, HIGH);
  digitalWrite(rele3, HIGH);
  digitalWrite(rele4, HIGH);
  digitalWrite(rele5, HIGH);
  digitalWrite(rele6, HIGH);
  digitalWrite(rele7, HIGH);
  digitalWrite(rele8, HIGH);
  digitalWrite(rel2e1, HIGH);
  digitalWrite(rel2e2, HIGH);
  digitalWrite(rel2e3, HIGH);
  digitalWrite(rel2e4, HIGH);
  digitalWrite(rel2e5, HIGH);
  digitalWrite(rel2e6, HIGH);
  digitalWrite(rel2e7, HIGH);
  digitalWrite(rel2e8, HIGH);

  //////////////////////////////// Při startu
  nactiteplotukohoutu();
}

void loop() {
  ////////////////////////////////////////////////////////////////Tlačítka
  //////////////////////////////// Čtení z pinů
  int tlacitko_ventilator = digitalRead(pin_tlacitko_ventilator);
  int tlacitko_dmychadlo = digitalRead(pin_tlacitko_dmychadlo);
  int tlacitko_obeh_TUV = digitalRead(pin_tlacitko_obeh_TUV);
  int tlacitko_podlaha_topeni = digitalRead(pin_tlacitko_podlaha_topeni);
  int tlacitko_dilna_radiatory = digitalRead(pin_tlacitko_dilna_radiatory);

  //////////////////////////////// Ventilátor
  if (tlacitko_ventilator == LOW) {
    if (ventilator_opakovani == true) {
      stav_ventilator = !stav_ventilator;
      ventilator_opakovani = false;
    }
  } else {
    ventilator_opakovani = true;
  }

  digitalWrite(pin_led_ventilator, stav_ventilator);

  //////////////////////////////// Dmychadlo
  if (tlacitko_dmychadlo == LOW) {
    if (dmychadlo_opakovani == true) {
      stav_dmychadlo = !stav_dmychadlo;
      dmychadlo_opakovani = false;
    }
  } else {
    dmychadlo_opakovani = true;
  }

  digitalWrite(pin_led_dmychadlo, stav_dmychadlo);

  //////////////////////////////// Oběh TUV
  if (tlacitko_obeh_TUV == LOW) {
    if (obeh_TUV_opakovani == true) {
      stav_obeh_TUV = !stav_obeh_TUV;
      obeh_TUV_opakovani = false;
    }
  } else {
    obeh_TUV_opakovani = true;
  }

  digitalWrite(pin_led_obeh_TUV, stav_obeh_TUV);

  //////////////////////////////// Podlaha topení
  if (tlacitko_podlaha_topeni == LOW) {
    if (podlaha_topeni_opakovani == true) {
      stav_podlaha_topeni = !stav_podlaha_topeni;
      podlaha_topeni_opakovani = false;
    }
  } else {
    podlaha_topeni_opakovani = true;
  }

  digitalWrite(pin_led_podlaha_topeni, stav_podlaha_topeni);

  //////////////////////////////// Dílna radiátory
  if (tlacitko_dilna_radiatory == LOW) {
    if (dilna_radiatory_opakovani == true) {
      stav_dilna_radiatory = !stav_dilna_radiatory;
      dilna_radiatory_opakovani = false;
    }
  } else {
    dilna_radiatory_opakovani = true;
  }

  digitalWrite(pin_led_dilna_radiatory, stav_dilna_radiatory);

  //////////////////////////////////////////////////////////////// Tlačítka funkce
  //////////////////////////////// Ventilátor
  if (stav_ventilator != ventilator_predchozi) {
    if (stav_ventilator == HIGH) {
      if (tlacitko_ventilator == LOW) {
        Serial1.print("bt0.val=1");
        Serial1.write(0xFF);
        Serial1.write(0xFF);
        Serial1.write(0xFF);
      }
      digitalWrite(rele1, LOW);
    }

    if (stav_ventilator == LOW) {
      if (tlacitko_ventilator == LOW) {
        Serial1.print("bt0.val=0");
        Serial1.write(0xFF);
        Serial1.write(0xFF);
        Serial1.write(0xFF);
      }
      digitalWrite(rele1, HIGH);
    }
    ventilator_predchozi = stav_ventilator;
  }

  //////////////////////////////// Dmychadlo
  if (stav_dmychadlo != dmychadlo_predchozi) {
    if (stav_dmychadlo == HIGH) {
      digitalWrite(rele2, LOW);
    }

    if (stav_dmychadlo == LOW) {
      digitalWrite(rele2, HIGH);
    }
    dmychadlo_predchozi = stav_dmychadlo;
  }

  //////////////////////////////// Oběh TUV
  if (stav_obeh_TUV != obeh_TUV_predchozi) {
    if (stav_obeh_TUV == HIGH) {
      if (tlacitko_obeh_TUV == LOW) {
        Serial1.print("bt1.val=1");
        Serial1.write(0xFF);
        Serial1.write(0xFF);
        Serial1.write(0xFF);
      }
      digitalWrite(rele3, LOW);
    }

    if (stav_obeh_TUV == LOW) {
      if (tlacitko_obeh_TUV == LOW) {
        Serial1.print("bt1.val=0");
        Serial1.write(0xFF);
        Serial1.write(0xFF);
        Serial1.write(0xFF);
      }
      digitalWrite(rele3, HIGH);
    }
    obeh_TUV_predchozi = stav_obeh_TUV;
  }

  //////////////////////////////// Podlaha topení
  if (stav_podlaha_topeni != podlaha_topeni_predchozi) {
    if (stav_podlaha_topeni == HIGH) {
      if (tlacitko_podlaha_topeni == LOW) {
        Serial1.print("bt3.val=1");
        Serial1.write(0xFF);
        Serial1.write(0xFF);
        Serial1.write(0xFF);
      }
      stranka_podlaha_topeni = true;
      digitalWrite(rele4, LOW);
    }

    if (stav_podlaha_topeni == LOW) {
      if (tlacitko_podlaha_topeni == LOW) {
        Serial1.print("bt3.val=0");
        Serial1.write(0xFF);
        Serial1.write(0xFF);
        Serial1.write(0xFF);
      }
      digitalWrite(rele4, HIGH);
      stranka_podlaha_topeni = false;
    }
    podlaha_topeni_predchozi = stav_podlaha_topeni;
  }

  //////////////////////////////// Dílna radiátory
  if (stav_dilna_radiatory != dilna_radiatory_predchozi) {
    if (stav_dilna_radiatory == HIGH) {
      Serial1.print("bt2.val=1");
      Serial1.write(0xFF);
      Serial1.write(0xFF);
      Serial1.write(0xFF);
      digitalWrite(rele5, LOW);
    }

    if (stav_dilna_radiatory == LOW) {
      Serial1.print("bt2.val=0");
      Serial1.write(0xFF);
      Serial1.write(0xFF);
      Serial1.write(0xFF);
      digitalWrite(rele5, HIGH);
    }
    dilna_radiatory_predchozi = stav_dilna_radiatory;
  }

  /*/////////////////////////////// Přepnutí auto manual
  if (tlacitko_podlaha_topeni == LOW && tlacitko_dilna_radiatory == LOW) {
    stav_tlacitko_auto_manual = false;
    Serial1.print("bt8.val=1");
    Serial1.write(0xFF);
    Serial1.write(0xFF);
    Serial1.write(0xFF);
  }

  /*/
  ////////////////////////////////////////////////////////////// Přímání z modulu MAX485
  //////////////////////////////// Kuchyně
  while (Serial2.available() > 0) {
    char c = Serial2.read();
    if (c == '\n') {
      if (buffer2.startsWith("T:")) {
        teplomer_kuchyn = buffer2.substring(2).toFloat();
      }
      buffer2 = "";
    } else {
      buffer2 += c;
    }
  }

  //////////////////////////////// Koupelny a pokoje
  while (Serial3.available() > 0) {
    char c = Serial3.read();
    if (c == '\n') {
      if (buffer3.startsWith("T:")) {
        int sep = buffer3.indexOf(';');
        if (sep > 0) {
          teplomer_pokoj = buffer3.substring(2, sep).toFloat();
          teplomer_koupelna = buffer3.substring(sep + 1).toFloat();
        }
      }
      buffer3 = "";
    } else {
      buffer3 += c;
    }
  }

  /*/////////////////////////////////////////////////////////////// Podmínky kohoutů
  if (stav_tlacitko_auto_manual == true) {
    kohout_mistnosti(KOUPELNA, teplomer_koupelna >= prednastavena_koupelna - 0.5, true);
    kohout_mistnosti(KUCHYN, teplomer_kuchyn >= prednastavena_kuchyn - 0.5, true);
    kohout_mistnosti(POKOJ, teplomer_pokoj >= prednastavena_pokoj - 0.5, true);
    stav_podlaha_topeni = (teplomer_koupelna >= prednastavena_koupelna - 0.5) || (teplomer_kuchyn >= prednastavena_kuchyn - 0.5) || (teplomer_pokoj >= prednastavena_pokoj - 0.5);
  }
  /*/
  ////////////////////////////////////////////////////////////// Podmínky stav nouze pro kotel :D
  if (teplota_kotle >= stavNouze_kotel && predchozi_stav == true) {
    predchozi_stav_dilna_radiatory = stav_dilna_radiatory;
    predchozi_stav_podlaha_topeni = stav_podlaha_topeni;
    //predchozi_stav_tlacitko_auto_manual = stav_tlacitko_auto_manual;

    stav_dilna_radiatory = true;
    stav_podlaha_topeni = true;
    //stav_tlacitko_auto_manual = false;

    kohout_mistnosti(KOUPELNA, false, true);
    kohout_mistnosti(KUCHYN, false, true);
    kohout_mistnosti(POKOJ, false, true);
    predchozi_stav = false;
  } else if (predchozi_stav == false) {
    lcd.setCursor(0, 0);
    lcd.print("  !STAV NOUZE!  ");
    lcd.setCursor(0, 1);
    lcd.print("Kotel: ");
    lcd.print(teplota_kotle);
    lcd.write(byte(0));
    lcd.print("C      ");
    if (teplota_kotle >= opravdu_stavNouze_kotel) {
      if (millis() - cas_sireny > 20000) {
        digitalWrite(rel2e8, LOW);
        delay(2000);
        digitalWrite(rel2e8, HIGH);
        cas_sireny = millis();
      }
    }
  } else if (teplota_kotle < stavNouze_kotel && predchozi_stav == false) {
    stav_dilna_radiatory = predchozi_stav_dilna_radiatory;
    stav_podlaha_topeni = predchozi_stav_podlaha_topeni;
    //stav_tlacitko_auto_manual = predchozi_stav_tlacitko_auto_manual;
    predchozi_stav = true;
  }

  //////////////////////////////////////////////////////////////// Přímání z nextion displeje
  if (Serial1.available() > 0) {
    byte znak = Serial1.read();
    //Serial.println(znak);
    //////////////////////////////// Přednastavená hodnota z nextion displaye
    if (znak == 'J') {
      rozsvit(true);
      if (Serial1.available()) {
        byte hodnota1 = Serial1.read();
        if (hodnota1 != 255) {
          Serial.print("přajitá hodnota z koupelny - ");
          Serial.println(hodnota1);
          prednastavena_koupelna = hodnota1;
        }
        nextionukonceni();
      }
    } else if (znak == 'K') {
      rozsvit(true);
      if (Serial1.available()) {
        byte hodnota2 = Serial1.read();
        if (hodnota2 != 255) {
          Serial.print("přajitá hodnota z Kuchyne - ");
          Serial.println(hodnota2);
          prednastavena_kuchyn = hodnota2;
        }
        nextionukonceni();
      }
    } else if (znak == 'L') {
      rozsvit(true);
      if (Serial1.available()) {
        byte hodnota3 = Serial1.read();
        if (hodnota3 != 255) {
          Serial.print("přajitá hodnota z Pokoje - ");
          Serial.println(hodnota3);
          prednastavena_pokoj = hodnota3;
        }
        nextionukonceni();
      }
      //////////////////////////////// Talčítka z nextion displaye
    } else if (znak != 0xFF && znak != 0x00) {
      switch (znak) {
        case 'A':
          rozsvit(true);
          //stranka_ventilator = true;
          stav_ventilator = true;
          nextionukonceni();
          break;

        case 'a':
          rozsvit(true);
          //stranka_ventilator = false;
          stav_ventilator = false;
          nextionukonceni();
          break;

        case 'B':
          rozsvit(true);
          //stranka_obeh_TUV = true;
          stav_obeh_TUV = true;
          nextionukonceni();
          break;

        case 'b':
          rozsvit(true);
          //stranka_obeh_TUV = false;
          stav_obeh_TUV = false;
          nextionukonceni();
          break;

        case 'C':
          rozsvit(true);
          //stranka_dilna_radiatory = true;
          stav_dilna_radiatory = true;
          nextionukonceni();
          break;

        case 'c':
          rozsvit(true);
          // stranka_dilna_radiatory = false;
          stav_dilna_radiatory = false;
          nextionukonceni();
          break;

        case 'D':
          rozsvit(true);
          stranka_podlaha_topeni = true;
          stav_podlaha_topeni = true;
          nextionukonceni();
          break;

        case 'd':
          rozsvit(true);
          stranka_podlaha_topeni = false;
          stav_podlaha_topeni = false;
          nextionukonceni();
          break;

        case 'E':
          rozsvit(true);
          stranka_kohout_koupelna = true;
          kohout_mistnosti(KOUPELNA, true, false);
          nextionukonceni();
          break;

        case 'e':
          rozsvit(true);
          stranka_kohout_koupelna = false;
          kohout_mistnosti(KOUPELNA, false, false);
          nextionukonceni();
          break;

        case 'F':
          rozsvit(true);
          stranka_kohout_kuchyn = true;
          kohout_mistnosti(KUCHYN, true, false);
          nextionukonceni();
          break;

        case 'f':
          rozsvit(true);
          stranka_kohout_kuchyn = false;
          kohout_mistnosti(KUCHYN, false, false);
          nextionukonceni();
          break;

        case 'G':
          rozsvit(true);
          stranka_kohout_pokoj = true;
          kohout_mistnosti(POKOJ, true, false);
          nextionukonceni();
          break;

        case 'g':
          rozsvit(true);
          stranka_kohout_pokoj = false;
          kohout_mistnosti(POKOJ, false, false);
          nextionukonceni();
          break;

        case 'H':
          rozsvit(true);
          stav_tlacitko_auto_manual = true;
          nextionukonceni();
          break;

        case 'h':
          rozsvit(true);
          stav_tlacitko_auto_manual = false;
          nextionukonceni();
          break;

        case 'I':
          rozsvit(true);
          nactiteplotukohoutu();
          prechod_stranek(0);
          nextionukonceni();
          break;

        case 'i':
          rozsvit(true);
          nactiteplotukohoutu();
          prechod_stranek(1);
          nextionukonceni();
          break;

        case 'M':
          rozsvit(true);
          nextionukonceni();
          break;
      }
    } 
  }

  //////////////////////////////////////////////////////////////// Teploměry časování
  if (millis() - cas_nacteni_teplomeru > 20000) {
    cas_nacteni_teplomeru = millis();
    nactiteplotukohoutu();
  }
  //////////////////////////////////////////////////////////////// Časování svitu
  if (millis() - cas_svitu > 60000) {
    cas_svitu = millis();
    rozsvit(false);
  }
  delay(15);
}

void kohout_mistnosti(Mistnosti mistnost, bool stav, bool pozice) {
  static bool stav_predchozi1 = false;
  static bool stav_predchozi2 = false;
  static bool stav_predchozi3 = false;

  switch (mistnost) {
    case KOUPELNA:
      if (stav != stav_predchozi1) {
        if (stav == true) {
          if (pozice == true) {
            Serial1.print("va0.val=1");
            Serial1.write(0xFF);
            Serial1.write(0xFF);
            Serial1.write(0xFF);
          }
          stranka_kohout_koupelna = true;
          digitalWrite(rele6, LOW);
        }

        if (stav == false) {
          if (pozice == true) {
            Serial1.print("va0.val=0");
            Serial1.write(0xFF);
            Serial1.write(0xFF);
            Serial1.write(0xFF);
          }
          stranka_kohout_koupelna = false;
          digitalWrite(rele6, HIGH);
        }
        stav_predchozi1 = stav;
      }
      break;

    case KUCHYN:
      if (stav != stav_predchozi2) {
        if (stav == true) {
          if (pozice == true) {
            Serial1.print("va1.val=1");
            Serial1.write(0xFF);
            Serial1.write(0xFF);
            Serial1.write(0xFF);
          }
          stranka_kohout_kuchyn = true;
          digitalWrite(rele7, LOW);
        }

        if (stav == false) {
          if (pozice == true) {
            Serial1.print("va1.val=0");
            Serial1.write(0xFF);
            Serial1.write(0xFF);
            Serial1.write(0xFF);
          }
          stranka_kohout_kuchyn = false;
          digitalWrite(rele7, HIGH);
        }
        stav_predchozi2 = stav;
      }
      break;

    case POKOJ:
      if (stav != stav_predchozi3) {
        if (stav == true) {
          if (pozice == true) {
            Serial1.print("va2.val=1");
            Serial1.write(0xFF);
            Serial1.write(0xFF);
            Serial1.write(0xFF);
          }
          stranka_kohout_pokoj = true;
          digitalWrite(rel2e1, LOW);
        }

        if (stav == false) {
          if (pozice == true) {
            Serial1.print("va2.val=0");
            Serial1.write(0xFF);
            Serial1.write(0xFF);
            Serial1.write(0xFF);
          }
          stranka_kohout_pokoj = false;
          digitalWrite(rel2e1, HIGH);
        }
        stav_predchozi3 = stav;
      }
      break;
  }
}



//////////////////////////////////////////////////////////////// Přechody mezi stránkami
void prechod_stranek(int stranka) {
  switch (stranka) {
    case 0:
      if (stranka_kohout_koupelna == true) {
        Serial1.print("p3.pic=61");
        Serial1.write(0xFF);
        Serial1.write(0xFF);
        Serial1.write(0xFF);
      } else if (stranka_kohout_koupelna == false) {
        Serial1.print("p3.pic=60");
        Serial1.write(0xFF);
        Serial1.write(0xFF);
        Serial1.write(0xFF);
      }
      if (stranka_kohout_kuchyn == true) {
        Serial1.print("p4.pic=61");
        Serial1.write(0xFF);
        Serial1.write(0xFF);
        Serial1.write(0xFF);
      } else if (stranka_kohout_kuchyn == false) {
        Serial1.print("p4.pic=60");
        Serial1.write(0xFF);
        Serial1.write(0xFF);
        Serial1.write(0xFF);
      }
      if (stranka_kohout_pokoj == true) {
        Serial1.print("p5.pic=61");
        Serial1.write(0xFF);
        Serial1.write(0xFF);
        Serial1.write(0xFF);
      } else if (stranka_kohout_pokoj == false) {
        Serial1.print("p5.pic=60");
        Serial1.write(0xFF);
        Serial1.write(0xFF);
        Serial1.write(0xFF);
      }
      if (stranka_podlaha_topeni == true) {
        Serial1.print("bt3.val=1");
        Serial1.write(0xFF);
        Serial1.write(0xFF);
        Serial1.write(0xFF);
      } else if (stranka_podlaha_topeni == false) {
        Serial1.print("bt3.val=0");
        Serial1.write(0xFF);
        Serial1.write(0xFF);
        Serial1.write(0xFF);
      }
      break;

    case 1:
      if (stranka_kohout_koupelna == true) {
        Serial1.print("va0.val=1");
        Serial1.write(0xFF);
        Serial1.write(0xFF);
        Serial1.write(0xFF);
      } else if (stranka_kohout_koupelna == false) {
        Serial1.print("va0.val=0");
        Serial1.write(0xFF);
        Serial1.write(0xFF);
        Serial1.write(0xFF);
      }
      if (stranka_kohout_kuchyn == true) {
        Serial1.print("va1.val=1");
        Serial1.write(0xFF);
        Serial1.write(0xFF);
        Serial1.write(0xFF);
      } else if (stranka_kohout_kuchyn == false) {
        Serial1.print("va1.val=0");
        Serial1.write(0xFF);
        Serial1.write(0xFF);
        Serial1.write(0xFF);
      }
      if (stranka_kohout_pokoj == true) {
        Serial1.print("va2.val=1");
        Serial1.write(0xFF);
        Serial1.write(0xFF);
        Serial1.write(0xFF);
      } else if (stranka_kohout_pokoj == false) {
        Serial1.print("va2.val=0");
        Serial1.write(0xFF);
        Serial1.write(0xFF);
        Serial1.write(0xFF);
      }
      if (stranka_podlaha_topeni == true) {
        Serial1.print("bt4.val=1");
        Serial1.write(0xFF);
        Serial1.write(0xFF);
        Serial1.write(0xFF);
      } else if (stranka_podlaha_topeni == false) {
        Serial1.print("bt4.val=0");
        Serial1.write(0xFF);
        Serial1.write(0xFF);
        Serial1.write(0xFF);
      }
      break;
  }
}
//////////////////////////////////////////////////////////////// Teploměry a posílání do nextion
void nactiteplotukohoutu() {
  lcd.clear();
  kotel.requestTemperatures();
  sud.requestTemperatures();
  teplota_kotel(kotel.getTempCByIndex(0));
  teplota_sud(sud.getTempCByIndex(0));
  sendToNextion("t7.txt", String(teplomer_koupelna));
  sendToNextion("t8.txt", String(teplomer_kuchyn));
  sendToNextion("t9.txt", String(teplomer_pokoj));
}

void teplota_sud(float teplo) {
  for (int i = 0; i < pocet_prav; i++) {
    if (teplo > 15) {  //opravená teplota
      i = pocet_prav;
    } else if (teplo < -80) {  //chybná teplota
      sud.requestTemperatures();
      teplo = sud.getTempCByIndex(0);
    }
  }
  sendToNextion("t6.txt", String(teplo));

  lcd.setCursor(0, 1);
  lcd.print("Sud: ");
  lcd.print(teplo);
  lcd.write(byte(0));
  lcd.print("C      ");
}

void teplota_kotel(float teplo) {
  for (int i = 0; i < pocet_prav; i++) {
    if (teplo > 15) {  //opravená teplota
      i = pocet_prav;
    } else if (teplo < -80) {  //chybná teplota
      kotel.requestTemperatures();
      teplo = kotel.getTempCByIndex(0);
    }
  }
  sendToNextion("t5.txt", String(teplo));

  teplota_kotle = teplo;
  lcd.setCursor(0, 0);
  lcd.print("Kotel: ");
  lcd.print(teplo);
  lcd.write(byte(0));
  lcd.print("C      ");
}
//////////////////////////////////////////////////////////////// Ukončení komunakace
void nextionukonceni() {
  Serial1.read();
  Serial1.read();
  Serial1.read();
}
//////////////////////////////////////////////////////////////// Odesílání teploty do nextion
void sendToNextion(String component, String value) {
  String command = component + "=\"" + value + "\"";
  Serial1.print(command);
  Serial1.write(0xFF);
  Serial1.write(0xFF);
  Serial1.write(0xFF);
}
//////////////////////////////////////////////////////////////// Chytrý jas
void rozsvit(bool svit) {
  if (svit == true) {
    cas_svitu = millis();
    Serial1.print("dim=100");
    Serial1.write(0xFF);
    Serial1.write(0xFF);
    Serial1.write(0xFF);
  } else if (svit == false) {
    Serial1.print("dim=5");
    Serial1.write(0xFF);
    Serial1.write(0xFF);
    Serial1.write(0xFF);
  }
}
