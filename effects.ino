#define TRACK_STEP 50
int TOP_INDEX = int(NUM_LEDS / 2);
//******************************* НОВАЯ РАДУГА ************************
int ihue = 0;
void new_rainbow_loop() {                     
  ihue -= 1;
  fill_rainbow( leds, NUM_LEDS, ihue );
  LEDS.show();
  delay(1);}
// ****************************** ночник v2 ***************************
void quad_bright_curve() {      
  int ax;
  for (int x = 0; x < NUM_LEDS; x++ ) {
    if (x <= TOP_INDEX) {
      ax = x;
    }
    else if (x > TOP_INDEX) {
      ax = NUM_LEDS - x;
    }
    int a = 1; int b = 1; int c = 0;
    int iquad = -(ax * ax * a) + (ax * b) + c; //-ax2+bx+c
    int hquad = -(TOP_INDEX * TOP_INDEX * a) + (TOP_INDEX * b) + c;
    ibright = int((float(iquad) / float(hquad)) * 255);
    leds[x] = CHSV(thishue, thissat, ibright);
  }
  LEDS.show();
  delay(50);
}

// ****************************** ОГОНЁК ******************************
int16_t position;
int posi;
int posi2;
boolean direction;

void lighter() {
  FastLED.clear();
  if (direction) {
    position++;
    if (position > NUM_LEDS - 2) {
      direction = false;
    }
  } else {
    position--;
    if (position < 1) {
      direction = true;
    }
  }
  leds[position] = CRGB::White;
}
// ****************************** КОМЕТА ********************************
void comet() {
  FastLED.clear();
  if (direction) {
    position++;
    posi = position - 1;
    posi2 = position - 2;
    if (position > NUM_LEDS - 2) {
      direction = false;
    }
  } else {
    position--;
    posi = position + 1;
    posi2 = position + 2;
    if (position < 1) {
      direction = true;
    }
  }
  leds[position] = CRGB::Blue;
  leds[posi] = CRGB(0, 0, 50);
}

// ****************************** СВЕТЛЯЧКИ ******************************
#define MAX_SPEED 30
#define BUGS_AMOUNT 3
int16_t speed[BUGS_AMOUNT];
int16_t pos[BUGS_AMOUNT];
CRGB bugColors[BUGS_AMOUNT];

void lightBugs() {
  if (loadingFlag) {
    loadingFlag = false;
    for (int i = 0; i < BUGS_AMOUNT; i++) {
      bugColors[i] = CHSV(random(0, 9) * 28, 255, 255);
      pos[i] = random(0, NUM_LEDS);
      speed[i] += random(-5, 6);
    }
  }
  FastLED.clear();
  for (int i = 0; i < BUGS_AMOUNT; i++) {
    speed[i] += random(-5, 6);
    if (speed[i] == 0) speed[i] += (-5, 6);

    if (abs(speed[i]) > MAX_SPEED) speed[i] = 0;
    pos[i] += speed[i] / 10;
    if (pos[i] < 0) {
      pos[i] = 0;
      speed[i] = -speed[i];
    }
    if (pos[i] > NUM_LEDS - 1) {
      pos[i] = NUM_LEDS - 1;
      speed[i] = -speed[i];
    }
    leds[pos[i]] = bugColors[i];
  }
}

// ****************************** ЦВЕТА ******************************
void colors() {
  hue += 2;
  CRGB thisColor = CHSV(hue, 255, 255);
  fillAll(CHSV(hue, 255, 255));
}

// ****************************** РАДУГА ******************************
void rainbow() {
  hue += 4;
  for (int i = 0; i < NUM_LEDS; i++)
    leds[i] = CHSV((byte)(hue + i * float(255 / NUM_LEDS)), 255, 255);
}
// ***************************** ночник *******************************
void night() {
hue += 1;
  for (int i = 0; i < NUM_LEDS; i++)
    leds[0] = CHSV((byte)(hue + i * float(255 / NUM_LEDS)), 255, 255);
}

// ****************************** КОНФЕТТИ ******************************
void sparkles() {
  byte thisNum = random(0, NUM_LEDS);
  if (getPixColor(thisNum) == 0)
    leds[thisNum] = CHSV(random(0, 255), 255, 255);
  fade();
}

// ****************************** ОГОНЬ ******************************
#define COOLING  55
// SPARKING: What chance (out of 255) is there that a new spark will be lit?
// Higher chance = more roaring fire.  Lower chance = more flickery fire.
// Default 120, suggested range 50-200.
#define SPARKING 180

void fire() {
  random16_add_entropy( random());
  Fire2012WithPalette(); // run simulation frame, using palette colors
}

void Fire2012WithPalette()
{
  // Array of temperature readings at each simulation cell
  static byte heat[NUM_LEDS];

  // Step 1.  Cool down every cell a little
  for ( int i = 0; i < NUM_LEDS; i++) {
    heat[i] = qsub8( heat[i],  random8(0, ((COOLING * 10) / NUM_LEDS) + 2));
  }

  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
  for ( int k = NUM_LEDS - 1; k >= 2; k--) {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
  }

  // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
  if ( random8() < SPARKING ) {
    int y = random8(7);
    heat[y] = qadd8( heat[y], random8(160, 255) );
  }

  // Step 4.  Map from heat cells to LED colors
  for ( int j = 0; j < NUM_LEDS; j++) {
    // Scale the heat value from 0-255 down to 0-240
    // for best results with color palettes.
    byte colorindex = scale8( heat[j], 240);
    CRGB color = ColorFromPalette( gPal, colorindex);
    int pixelnumber;
    if ( gReverseDirection ) {
      pixelnumber = (NUM_LEDS - 1) - j;
    } else {
      pixelnumber = j;
    }
    leds[pixelnumber] = color;
  }
}
// ****************************** ОГОНЬ ****1**************************
#define COOLING  55
// SPARKING: What chance (out of 255) is there that a new spark will be lit?
// Higher chance = more roaring fire.  Lower chance = more flickery fire.
// Default 120, suggested range 50-200.
#define SPARKING 180

void fire1() {
  random16_add_entropy( random());
  Fire2013WithPalette(); // run simulation frame, using palette colors
}

void Fire2013WithPalette()
{
  // Array of temperature readings at each simulation cell
  static byte heat[NUM_LEDS];

  // Step 1.  Cool down every cell a little
  for ( int i = 0; i < NUM_LEDS; i++) {
    heat[i] = qsub8( heat[i],  random8(0, ((COOLING * 10) / NUM_LEDS) + 2));
  }

  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
  for ( int k = NUM_LEDS - 1; k >= 2; k--) {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
  }

  // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
  if ( random8() < SPARKING ) {
    int y = random8(7);
    heat[y] = qadd8( heat[y], random8(160, 255) );
  }

  // Step 4.  Map from heat cells to LED colors
  for ( int j = 0; j < NUM_LEDS; j++) {
    // Scale the heat value from 0-255 down to 0-240
    // for best results with color palettes.
    byte colorindex = scale8( heat[j], 240);
    CRGB color = ColorFromPalette( gPal1, colorindex);
    int pixelnumber;
    if ( gReverseDirection ) {
      pixelnumber = (NUM_LEDS - 1) - j;
    } else {
      pixelnumber = j;
    }
    leds[pixelnumber] = color;
  }
}

// *************** ВИНИГРЕТ ***************
void vinigret() {
  for (int i = 0; i < NUM_LEDS; i++) {
    if ((uint32_t)getPixColor(i) == 0) {
      
    }
  }
}


// ****************** СЛУЖЕБНЫЕ ФУНКЦИИ *******************


// яркость v2

void light(){
  if (!powerActive) {
      effectTimer.stop();
      w = true;
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

void fade() {
  for (int i = 0; i < NUM_LEDS; i++) {
    if ((uint32_t)getPixColor(i) == 0) continue;
    leds[i].fadeToBlackBy(TRACK_STEP);

    /*// измеряяем цвет текущего пикселя
      uint32_t thisColor = getPixColor(i);

      // если 0, то пропускаем действия и переходим к следующему
      if (thisColor == 0) continue;

      // разбиваем цвет на составляющие RGB
      byte rgb[3];
      rgb[0] = (thisColor >> 16) & 0xff;
      rgb[1] = (thisColor >> 8) & 0xff;
      rgb[2] = thisColor & 0xff;

      // ищем максимум
      byte maximum = max(max(rgb[0], rgb[1]), rgb[2]);
      float coef = 0;

      // если есть возможность уменьшить
      if (maximum >= TRACK_STEP)
      // уменьшаем и находим коэффициент уменьшения
      coef = (float)(maximum - TRACK_STEP) / maximum;

      // далее все цвета умножаем на этот коэффициент
      for (int i = 0; i < 3; i++) {
      if (rgb[i] > 0) rgb[i] = (float)rgb[i] * coef;
      else rgb[i] = 0;
      }
      leds[i] = CRGB(rgb[0], rgb[1], rgb[2]);*/
  }
}
