#include "DHT.h"
#include <Wire.h>

// DHT Sensor at Pin 2
#define DHTPIN 2

// DHT version 11 
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

byte buff[2];

// i2c Addresse
int BH1750_address = 0x23;

// Gas sensor at A0
int gasSensor = A0;

// Light LED at A7
int outLight = A7;

void setup() {
  Wire.begin();
  BH1750_Init(BH1750_address);

  // set LED as output
  pinMode(outLight, OUTPUT);
  
  // wait a bit
  delay(200);

  // Start bus with 9600
  Serial.begin(9600);
  dht.begin();
}

void loop() {
   checkGas();
   checkLight();
   checkHumidAndTemp();
   Serial.println();
   delay(1000);
}

void checkLight() {
  Serial.print("light: ");
  float valf = 0;
  if (BH1750_Read(BH1750_address) == 2) {
    valf = ((buff[0] << 8) | buff[1]) / 1.2;
    // too much light
    if (valf < 0) {
      Serial.print("> 65535");
    } else {
      Serial.print((int)valf,DEC); 
    }
    Serial.print(" lx. \t");
  } 
}

void checkHumidAndTemp() {
   float h = dht.readHumidity();
   float t = dht.readTemperature();

   // Invalid data
   if (isnan(t) || isnan(h)) {
      Serial.println("Failed to read from DHT");
   } else {
      Serial.print("Humidity: "); 
      Serial.print(h);
      Serial.print(" %\t");
      Serial.print("Temperature: "); 
      Serial.print(t);
      Serial.print(" *C \t");
   } 
}

void checkGas() {
  // read value form analog
  int gas = analogRead(gasSensor);
  Serial.print("Gas: ");
  Serial.print(gas);
  Serial.print(" \t");
  if ( gas > 500 ) {
    digitalWrite(outLight, HIGH);
  } else {
    digitalWrite(outLight, LOW);
  } 
}

void BH1750_Init(int address){
  Wire.beginTransmission(address);
  Wire.write(0x10); // 1 [lux] aufloesung
  Wire.endTransmission();
}

byte BH1750_Read(int address){ 
  byte i = 0;
  Wire.beginTransmission(address);
  Wire.requestFrom(address, 2);
  while(Wire.available()){
    buff[i] = Wire.read(); 
    i++;
  }
  Wire.endTransmission();  
  return i;
}