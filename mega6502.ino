#include <Arduino.h>
#include <avr/pgmspace.h>
#include <LiquidCrystal.h>
#include "AddressSpace.h"

#define CLOCK 53
#define RESET 52
#define RW 51

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

// Freetronics
LiquidCrystal lcd( 8, 9, 4, 5, 6, 7 );
// DFRobot
//LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
AddressSpace mem;

// these constants won't change.  But you can change the size of
// your LCD using them:
const int numRows = 2;
const int numCols = 16;

void setup() {
  // set up the LCD's number of columns and rows:
  lcd.begin(numCols, numRows);
  Serial.begin(9600);
  Serial.println("RESET");
  
  pinMode(CLOCK, OUTPUT);
  pinMode(RESET, OUTPUT);
  pinMode(RW, INPUT);
  digitalWrite(RESET, HIGH);

  // setup PORTC to read the low byte of the address
  DDRC = 0x00;
  PORTC = 0x00;
  // setup PORTA to do the same for the high byte
  DDRA = 0x00;
  PORTA = 0x00;

  // send reset pulse
  digitalWrite(RESET, LOW);
  digitalWrite(CLOCK, HIGH);
  digitalWrite(CLOCK, LOW);
  digitalWrite(CLOCK, HIGH);
  digitalWrite(CLOCK, LOW);
  digitalWrite(CLOCK, HIGH);
  digitalWrite(CLOCK, LOW);
  digitalWrite(CLOCK, HIGH);
  digitalWrite(CLOCK, LOW);
  digitalWrite(CLOCK, HIGH);
  digitalWrite(CLOCK, LOW);
  digitalWrite(CLOCK, HIGH);
  digitalWrite(RESET, HIGH);
}

void loop() {
  uint8_t addrl, addrh;
  cli();
  cbi(PORTB, 0);
  __asm__("nop\n\t""nop\n\t""nop\n\t""nop\n\t"); // 4 * 62.5ns delay @ 16mhz
  __asm__("nop\n\t""nop\n\t""nop\n\t""nop\n\t");
  addrl = PINA;
  addrh = PINC;
  sbi(PORTB, 0);
  sei();
  switch (digitalRead(RW)) {
    case HIGH:
      // read cycle
      DDRL = 0xff;
      PORTL = mem.Read((addrh << 8) | addrl);
      break;
    case LOW:
      // write cycle
      DDRL = 0x00;
      PORTL = 0;
      mem.Write((addrh << 8) | addrl, PINL);
  }
  delay(50);
}
