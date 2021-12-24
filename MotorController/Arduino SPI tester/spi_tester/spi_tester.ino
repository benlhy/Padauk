/*
 * This is the Padauk SPI tester
 */

#include <SPI.h>


void setup() {
  Serial.begin(9600);
  Serial.println("Serial Started");
  SPI.begin(); // start SPI
  SPI.setClockDivider(SPI_CLOCK_DIV8);
  // put your setup code here, to run once:
  pinMode(13,OUTPUT);
  pinMode(4,OUTPUT);

}

void loop() {
  
  digitalWrite(13,HIGH);
  digitalWrite(4,LOW);
  Serial.println("Waiting for user input");
  while (Serial.available() == 0);
  uint16_t value = 0;
  value = Serial.readStringUntil("\r").toInt();
  SPI.beginTransaction(SPISettings(1600, MSBFIRST, SPI_MODE3));
  SPI.transfer(0x05);
  SPI.transfer(value>>8);
  SPI.transfer(value & 0xFF);
  Serial.print("Sent: ");
  Serial.println(value);
  digitalWrite(13,LOW);
  digitalWrite(4,HIGH);
  delay(500);
  
  // put your main code here, to run repeatedly:

}
