/*
  Скетч к проекту "Эффектный светильник"
  Страница проекта (схемы, описания): https://alexgyver.ru/gyverlight/
  Исходники на GitHub: https://github.com/AlexGyver/gyverlight/
  Нравится, как написан код? Поддержи автора! https://alexgyver.ru/support_alex/
  Автор: AlexGyver Technologies, 2018
  https://AlexGyver.ru/
*/

/*
   Управление кнопкой/сенсором
  - Удержание - яркость
  - 1х тап - вкл/выкл
  - 2х тап - переключ режима
  - 3х тап - вкл/выкл белый свет
  - 4х тап - старт/стоп авто смены режимов
*/

/*
   Версия 1.3 - пофикшен баг с fillAll
*/

// ************************** НАСТРОЙКИ ***********************
#define CURRENT_LIMIT 3000  // лимит по току в миллиамперах, автоматически управляет яркостью (пожалей свой блок питания!) 0 - выключить лимит
#define AUTOPLAY_TIME 60    // время между сменой режимов в секундах

#define NUM_LEDS 13         // количсетво светодиодов в одном отрезке ленты
#define NUM_STRIPS 4        // количество отрезков ленты (в параллели)
#define LED_PIN 6           // пин ленты
#define BTN_PIN 2           // пин кнопки/сенсора
#define MIN_BRIGHTNESS 5  // минимальная яркость при ручной настройке
#define BRIGHTNESS 200      // начальная яркость
#define FIRE_PALETTEE 1 
#define FIRE_PALETTE 1      // разные типы огня (0 - 3). Попробуй их все! =)

// ************************** ДЛЯ РАЗРАБОТЧИКОВ ***********************
#define MODES_AMOUNT 7 // количество режимов
#define COLORS_AMOUNT 14 // однотипных цветов

#include "GyverButton.h"
GButton touch(BTN_PIN, LOW_PULL, NORM_OPEN);

#include <FastLED.h>
CRGB leds[NUM_LEDS];
CRGBPalette16 gPal;
CRGBPalette16 gPal1;
CRGB Colors [] = {CRGB::White, CRGB::Violet, CRGB::BlueViolet, CRGB::MediumBlue, CRGB::DarkTurquoise, CRGB::LimeGreen, CRGB::Lime, CRGB::YellowGreen, CRGB::Gold, CRGB::Orange, CRGB::OrangeRed, CRGB::Red, CRGB::Crimson, CRGB::DarkMagenta};

#include "GyverTimer.h"
GTimer_ms effectTimer(60);
GTimer_ms autoplayTimer((long)AUTOPLAY_TIME * 1000);
GTimer_ms brightTimer(20);

int brightness = BRIGHTNESS;
int tempBrightness;
byte thisMode;
byte thisColor;

bool gReverseDirection = false;
boolean loadingFlag = true;
boolean autoplay = false;
boolean powerDirection = true;
boolean powerActive = false;
boolean powerState = true;
boolean whiteMode = false;
boolean brightDirection = true;
boolean wasStep = false;


// залить все
void fillAll(CRGB newcolor) {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = newcolor;
  }
}

// функция получения цвета пикселя по его номеру
uint32_t getPixColor(int thisPixel) {
  return (((uint32_t)leds[thisPixel].r << 16) | ((long)leds[thisPixel].g << 8 ) | (long)leds[thisPixel].b);
}

void setup() {
  Serial.begin(9600);
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
          }
        }
        break;
      case 3: if (!powerActive) {
          whiteMode = !whiteMode;
          if (whiteMode) {
            effectTimer.stop();
            fillAll(CRGB::White);
            FastLED.show();
          } else {
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
    if (!powerActive) {
      effectTimer.stop();
      wasStep = true;
      if (brightDirection) {
      brightness += 5;
      int led1 = 255 / NUM_LEDS; // 100%
      int led2 = brightness / led1;
      for (int i = 0; i < led2; i++) {
        leds[i] = CRGB::Green;
        FastLED.show();
        for (int j = led2; j < NUM_LEDS; j++) {
          leds[j] = CRGB(0, 0, 0);
          //effectTimer.stop();
          FastLED.show();
          
        }
      }
   }
      else {
        brightness -= 5;
        int led1 = 255 / NUM_LEDS; // 100%
        int led2 = brightness / led1;
        for (int i = 0; i < led2; i++) {
          leds[i] = CRGB::Green;
          //effectTimer.stop();
          FastLED.show();
          for (int i = led2; i < NUM_LEDS; i++) {
            leds[i] = CRGB(0, 0, 0);
            //effectTimer.stop();
            FastLED.show();
          }
       }
      }
      brightness = constrain(brightness, MIN_BRIGHTNESS, 255);
      FastLED.setBrightness(brightness);
      FastLED.show();
      
      }
  }

    
  
  else{
    effectTimer.start();
  }
    
    

  if (touch.isRelease()) {
    if (wasStep) {
      wasStep = false;
      brightDirection = !brightDirection;
    }
  }

  if (effectTimer.isReady() && powerState) {
    switch (thisMode) {
      case 0: rainbow();
        break;
      case 1: colors();
        break;
      case 2: fire();
        break;
      case 3: fire1();
        break;
      case 4: lightBugs();
        break;
      case 5: sparkles();
        break;
      case 6: night();
        break;
      case 7: vinigret();
        break;

    }
    FastLED.show();
  }

   if (whiteMode && powerState) {
     fillAll(Colors[thisColor]);
    FastLED.show();
  }

  if (autoplayTimer.isReady() && autoplay) {    // таймер смены режима
    nextMode();
  }

  brightnessTick();
}

void nextMode() {
  thisMode++;
  if (thisMode >= MODES_AMOUNT) thisMode = 0;
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
        tempBrightness += 10;
        if (tempBrightness > brightness) {
          tempBrightness = brightness;
          powerActive = false;
        }
      } else {
        tempBrightness -= 10;
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
