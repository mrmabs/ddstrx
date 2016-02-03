#include <AD9850SPI.h>
#include <SPI.h>
#include <LiquidCrystal.h>

const int W_CLK_PIN = 13;
const int FQ_UD_PIN = 7;
const int RESET_PIN = 8;

const int BUTTON_UP = A0;
const int BUTTON_DOWN = A1;
const int BUTTON_SELECT = A2;

//const int TX_PIN = 4; // eventually want to control transmit from here

const int STATE_10HZ = 0;
const int STATE_100HZ = 1;
const int STATE_1KHZ = 2;
const int STATE_10KHZ = 3;
const int STATE_BAND = 4;
const int MAX_STATE = 4;

const int BAND_80 = 0;
const int BAND_40 = 1;
const int BAND_30 = 2;
const int BAND_20 = 3;
const int MAX_BAND = 3;

double freqStep = 1000;
int modeState = 2;
int band = 1;

double freqs[] = {3550000, 7090000, 10100000, 14300000};
double oldFreq = 0;
//double trimFreq = 124999600; // #0519
//double trimFreq = 125000750; // #0408
double trimFreq = 124999280; // #0249
int phase = 0;

LiquidCrystal lcd(0, 1, 2, 3, 6, 9);

void setup(){
  lcd.begin(8, 2);
  lcd.print("init");

  DDS.begin(W_CLK_PIN, FQ_UD_PIN, RESET_PIN);
  DDS.calibrate(trimFreq);

  //Serial.begin(9600);  
  //Serial.println("--- AD9850 Start ---");
  //Serial.println(); 

  pinMode(BUTTON_UP, INPUT_PULLUP);
  pinMode(BUTTON_DOWN, INPUT_PULLUP);
  pinMode(BUTTON_SELECT, INPUT_PULLUP);

  memMax = sizeof(mems);
}

void loop(){
  //delay(500);

  freq = freqs[band];
  // Display updates below
  if (freq != oldFreq) {
    // we shouldn't do these unless we have an update worth sending
    // lcd updates are creating noise in the reciever, but that could be building on a breadboard
    lcd.setCursor(0, 0);
    lcd.print(freq / 1000);
    lcd.print("   ");
    oldFreq = freq;
  }

  // DDS for some reason needs a little more time to get started, we'll keep pumping updates to it
  DDS.setfreq(freq, phase);
  
  // arbitrary wait time
  delay(200);

  // look for key press
  if (digitalRead(BUTTON_UP) == LOW) {
    freqs[band] = freqs[band] + freqStep;
    if (modeState == STATE_BAND) {
      band = band + 1;
      if (band > MAX_BAND) {
        band = 0;
      }
    }
  }
  if (digitalRead(BUTTON_DOWN) == LOW) {
    freqs[band] = freqs[band] - freqStep;
    if (modeState == STATE_BAND) {
      band = band - 1;
      if (band < 0) {
        band = MAX_BAND;
      }
    }
  }
  if (digitalRead(BUTTON_SELECT) == LOW) {
    modeState = modeState + 1;
    if (modeState > MAX_STATE) {
      modeState = 0;
    }
    switch (modeState) {
      case STATE_10HZ:
        freqStep = 10;
        lcd.setCursor(3, 1);
        lcd.print(" 10Hz");
        break;
      case STATE_100HZ:
        freqStep = 100;
        lcd.setCursor(3, 1);
        lcd.print("100Hz");
        break;
      case STATE_1KHZ:
        freqStep = 1000;
        lcd.setCursor(3, 1);
        lcd.print(" 1kHz");
        break;
      case STATE_10KHZ:
        freqStep = 10000;
        lcd.setCursor(3, 1);
        lcd.print("10kHz");
        break;
      case STATE_BAND:
        freqStep = 0;
        lcd.setCursor(3, 1);
        lcd.print(" Band");
        break;
    }
  }
}
