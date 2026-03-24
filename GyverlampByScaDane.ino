/*
  Скетч к проекту "Эффектный светильник"
  https://t.me/xaker456 // по всем вопросам и предложениям сюда
*/

/*
   Управление кнопкой/сенсором
  - Удержание - яркость
  - 1х тап - вкл/выкл
  - 2х тап - переключ режима / переключение цветов если зайти в белый режим
  - 3х тап - вкл/выкл белый свет 
  - 4х тап - старт/стоп авто смены режимов
*/

/*
   Версия beta v1.2
*/

// ************************** НАСТРОЙКИ ***********************
#define CURRENT_LIMIT 0  // лимит по току в миллиамперах, автоматически управляет яркостью (пожалей свой блок питания!) 0 - выключить лимит
#define AUTOPLAY_TIME 60    // время между сменой режимов в секундах

#define NUM_LEDS 13         // количсетво светодиодов в одном отрезке ленты
#define NUM_STRIPS 4        // количество отрезков ленты (в параллели)
#define LED_PIN 6           // пин ленты
#define BTN_PIN 2           // пин кнопки/сенсора
#define MIN_BRIGHTNESS 5    // минимальная яркость при ручной настройке
#define BRIGHTNESS 150      // начальная яркость
#define FIRE_PALETTE 1      // разные типы огня (0 - 3). Попробуй их все! =)
#define COMPORT 1           // Включить считывание из ком порта
/*
 1 - Включить лампу
 0 - Выключить лампу
 */
boolean autoplay = false;   // включить / выключить автосмену режимов, можно включть четырех кратным нажатием



// ************************** ДЛЯ РАЗРАБОТЧИКОВ ***********************


#define MODES_AMOUNT 8 // количество режимов
#define COLORS_AMOUNT 16 // однотипных цветов
#define FIRE_PALETTEE 1 

#include "GyverButton.h"
GButton touch(BTN_PIN, LOW_PULL, NORM_OPEN);

#include <FastLED.h>
CRGB leds[NUM_LEDS];
CRGBPalette16 gPal;
CRGBPalette16 gPal1;
CRGB Colors [] = {CRGB::White, CRGB::Violet, CRGB::BlueViolet, CRGB::MediumBlue, CRGB::DarkTurquoise, CRGB::LimeGreen, CRGB::Lime, CRGB::YellowGreen, CRGB::Gold, CRGB::Orange, CRGB::OrangeRed, CRGB::Red, CRGB::Crimson, CRGB::DarkMagenta, CRGB::RoyalBlue, CRGB::FairyLight};

#include "GyverTimer.h"
GTimer_ms effectTimer(60);
GTimer_ms autoplayTimer((long)AUTOPLAY_TIME * 1000);
GTimer_ms brightTimer(20);

int brightness = BRIGHTNESS;
int tempBrightness;
byte thisMode;
byte thisColor = 0;
// ----------------- НЕ ТРОГАТЬ :) --------------------
bool gReverseDirection = false;
boolean loadingFlag = true;
boolean powerDirection = true;
boolean powerActive = false;
boolean powerState = true;
boolean whiteMode = false;
boolean brightDirection = true;
boolean wasStep = false;
boolean w = false;
byte hue;
int ibright = 0;
int thishue = 180;             
int thissat = 255;
// --------------------------------------------------


// залить все
void fillAll(CRGB newcolor) {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = newcolor;
  }
}

void fillAllefect(CRGB newcolor){
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = newcolor;
    delay(20);
    FastLED.show();
  }
  }


// функция получения цвета пикселя по его номеру
uint32_t getPixColor(int thisPixel) {
  return (((uint32_t)leds[thisPixel].r << 16) | ((long)leds[thisPixel].g << 8 ) | (long)leds[thisPixel].b);
}
  
  
void setup() {
  if(COMPORT){
  Serial.begin(115200);
  }
  FastLED.addLeds<WS2811, LED_PIN, GRB>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  if (CURRENT_LIMIT > 0) FastLED.setMaxPowerInVoltsAndMilliamps(5, CURRENT_LIMIT / NUM_STRIPS);
  FastLED.setBrightness(brightness);
  FastLED.show();

  randomSeed(analogRead(0));
  touch.setTimeout(300);
  touch.setStepTimeout(50);
  
  if (FIRE_PALETTEE == 1) gPal1 = CRGBPalette16( CRGB::Black, CRGB::Indigo, CRGB::Magenta,  CRGB::Violet  );

  if (FIRE_PALETTE == 0) gPal = HeatColors_p;
  else if (FIRE_PALETTE == 1) gPal = CRGBPalette16( CRGB::Black, CRGB::Red, CRGB::OrangeRed, CRGB::Gold);
  else if (FIRE_PALETTE == 2) gPal = CRGBPalette16( CRGB::Black, CRGB::Blue, CRGB::Aqua,  CRGB::White);
  else if (FIRE_PALETTE == 3) gPal = CRGBPalette16( CRGB::Black, CRGB::Red, CRGB::White);


}

void loop() {
      if (COMPORT){
      int  command = Serial.read();
      if (command == '0') { 
        powerDirection = !powerDirection;
        powerActive =true; 
        tempBrightness = brightness * !powerDirection;
       
      }
       if (command == '1') { 
        powerDirection = true;
        powerActive =true;
        tempBrightness = brightness * !powerDirection;
       }
      }




  
  touch.tick();
  if (touch.hasClicks()) {
    byte clicks = touch.getClicks();
    switch (clicks) {
      case 1:
        powerDirection = !powerDirection;
        powerActive = true;
        tempBrightness = brightness * !powerDirection;
        break;

      case 2: if (!whiteMode && !powerActive) {
          nextMode();
        } 
        else 
        {
          if (whiteMode && !powerActive)
          {
            nextColor();
            if (whiteMode && powerState) {
            fillAllefect(Colors[thisColor]);
             }
          }
        }
        break;
      case 3: if (!powerActive) {
          whiteMode = !whiteMode;
          if (whiteMode) {
            effectTimer.stop();
            fillAll(Colors[thisColor]);
            FastLED.show();
          } else {
            FastLED.clear();
            effectTimer.start();
          }
        }
        break;
      case 4: if (!whiteMode && !powerActive) autoplay = !autoplay;
        break;
      default:
        break;
    }
  }

  if (touch.isStep()) {
    light();
  }
    

    
  
  else{
    if (w){
    if (whiteMode){
      fillAll(Colors[thisColor]);
      FastLED.show();
      w = false;
      }
    if(!whiteMode){
      FastLED.clear();
      effectTimer.start();
      w = false;  
      }

    
    

     
  }}
    
    

  if (touch.isRelease()) {
    if (wasStep) {
      wasStep = false;
      brightDirection = !brightDirection;
    }
  }

  if (effectTimer.isReady() && powerState) {
    switch (thisMode) {
      case 0: new_rainbow_loop();
        break;
      case 1: rainbow();
        break;
      case 2: colors();
        break;
      case 3: fire();
        break;
      case 4: fire1();
        break;
      case 5: lightBugs();
        break;
      case 6: sparkles();
        break;
      case 7: quad_bright_curve();
        break;
      case 8: comet();
        break;

    }
    FastLED.show();
  }

   //if (whiteMode && powerState) {
     //fillAll(Colors[thisColor]);
    //FastLED.show();
 // }

  if (autoplayTimer.isReady() && autoplay) {    // таймер смены режима
    nextMode();
  }

  brightnessTick();
}

void nextMode() {
  thisMode++;
  if (thisMode > MODES_AMOUNT) thisMode = 0;
  loadingFlag = true;
  FastLED.clear();
}

void nextColor() {
  thisColor++;
  if (thisColor >= COLORS_AMOUNT) thisColor = 0;
  loadingFlag = true;
  FastLED.clear();
}

void brightnessTick() {
  if (powerActive) {
    if (brightTimer.isReady()) {
      if (powerDirection) {
        powerState = true;
        tempBrightness += 5;
        if (tempBrightness > brightness) {
          tempBrightness = brightness;
          powerActive = false;
        }
      } else {
        tempBrightness -= 5;
        if (tempBrightness < 0) {
          tempBrightness = 0;
          powerActive = false;
          powerState = false;
        }
      }
      FastLED.setBrightness(tempBrightness);
      FastLED.show();
    }
  }
}
