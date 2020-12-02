// Nokia 5110 Temperature logger by Wunderwaffez. 
// LCD5110_Graph.h by Henning Karlsen

#include <LCD5110_Graph.h>
#include <OneWire.h>
#include <Wire.h>

LCD5110 lcd(8,9,10,11,12);
int DS18S20_Pin = A0;
OneWire ds(DS18S20_Pin);

extern uint8_t TinyFont[]; 
extern uint8_t MediumNumbers[];
extern uint8_t SmallFont[];

unsigned long interval;
long log_interval;
int temp;
int xr;
int x;
int xbar;
int percent;

int btn_prev1;
int btn_prev2;

void setup()
{
  Wire.begin();
  Serial.begin(9600);
  lcd.InitLCD();
  lcd.setFont(TinyFont);
   
  pinMode(3, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  pinMode(6, OUTPUT);

  btn_prev1 = digitalRead(3);
  btn_prev2 = digitalRead(4);
   
}

void loop() //Start Screen
{

  while (digitalRead(5) == HIGH) { //allow changing the value of interval until "ok" button is pressed
    
  lcd.update();
  float temperature = getTemp();

  buton();
  
  interval = constrain(interval, 0, 142);
  if (interval == 142) {interval = 140;}
  if (interval == 141) {interval = 0;}
  xbar = map(interval,0,140,5,78);
  
  lcd.setFont(SmallFont);
  lcd.drawRect(0,0,83,47);
  lcd.drawLine(0,11,83,11);
  lcd.print("Log Interval", CENTER, 2);

  lcd.setFont(MediumNumbers);
  lcd.printNumI(interval, CENTER, 17);

  if (interval < 10) {

  lcd.setFont(TinyFont);
  lcd.print("min", 52, 23);

  }

  else if (interval < 100) {

  lcd.setFont(TinyFont);
  lcd.print("min", 58, 23);

  }

  else if (interval > 99) {

  lcd.setFont(TinyFont);
  lcd.print("min", 63, 23);

  }

  lcd.drawRect(5,43,78,38);

  lcd.drawLine(5,42,xbar,42);
  lcd.drawLine(5,41,xbar,41);
  lcd.drawLine(5,40,xbar,40);
  lcd.drawLine(5,39,xbar,39);
  
  log_interval = interval * 60 * 1000;

 }

  lcd.clrScr();
  tone(6, 750, 100);  
  graph();

}

  
void buton() { //check the buttons and increase/decrease the value of interval.

  if (digitalRead(3) == LOW && btn_prev1 == HIGH)  {
    ++interval;
    tone(6, 750, 100);
    lcd.clrScr();
    lcd.update();
  }

  if (digitalRead(4) == LOW && btn_prev2 == HIGH)  {
    --interval;
    tone(6, 750, 100);
    lcd.clrScr();
    lcd.update();
  } 

  btn_prev1 = digitalRead(3);
  btn_prev2 = digitalRead(4);
  
}

void graph() { // Check the sensor and plot the graph

  
  
  x = xr+5;
  while ( x < 78) {

  lcd.setFont(TinyFont);
  lcd.drawLine(0, 0, 0, 48);
  lcd.drawLine(0, 47, 83, 47);
  lcd.drawLine(83, 0, 83, 48);
  
  lcd.drawLine(0, 8, 3, 8);
  lcd.drawLine(0, 13, 3, 13);
  lcd.drawLine(0, 18, 3, 18);
  lcd.drawLine(0, 23, 3, 23);
  lcd.drawLine(0, 28, 3, 28);  
  lcd.drawLine(0, 33, 3, 33);  
  lcd.drawLine(0, 38, 3, 38);
  lcd.drawLine(0, 43, 3, 43);  

  lcd.drawLine(83, 8, 81, 8);
  lcd.drawLine(83, 13, 81, 13);
  lcd.drawLine(83, 18, 81, 18);
  lcd.drawLine(83, 23, 81, 23);
  lcd.drawLine(83, 28, 81, 28);
  lcd.drawLine(83, 33, 81, 33);
  lcd.drawLine(83, 38, 81, 38);
  lcd.drawLine(83, 43, 81, 43);

  lcd.drawLine(0,6,83,6);
  
  float temperature = getTemp();
  temp = map(temperature,0,39,47,8);
  percent = map(x,5,77,0,100);

  lcd.drawLine(x,48,x,temp+1);
  lcd.printNumF(temperature, 1, CENTER, 0);
  lcd.printNumI(interval,LEFT, 0);

  if (interval < 10) {lcd.print("min", 5, 0);}
  else if (interval < 100) {lcd.print("min", 9, 0);}
  else if (interval > 99) {lcd.print("min", 14, 0);};

  Serial.println(temperature);
 
  if (percent < 100) {

    lcd.print("%", 70, 0);
    
  }
  else {

    lcd.print("%", 67, 0);
    
  }
  
  lcd.printNumI(percent, RIGHT, 0);
  
  lcd.update();
  xr++;
  x++;

  delay(log_interval);
  
  }

    
  while (digitalRead(5) == HIGH)  {

    x = 0;
    xr = 0;
    lcd.clrScr();
    
  }
  tone(6, 750, 100);
  delay(250);

}

float getTemp() {
  
  //returns the temperature from one DS18S20 in DEG Celsius

  byte data[12];
  byte addr[8];

  if ( !ds.search(addr)) {
      //no more sensors on chain, reset search
      ds.reset_search();
      return -1000;
  }

  if ( OneWire::crc8( addr, 7) != addr[7]) {
      Serial.println("CRC is not valid!");
      return -1000;
  }

  if ( addr[0] != 0x10 && addr[0] != 0x28) {
      Serial.print("Device is not recognized");
      return -1000;
  }

  ds.reset();
  ds.select(addr);
  ds.write(0x44,1); // start conversion, with parasite power on at the end

  byte present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE); // Read Scratchpad

  for (int i = 0; i < 9; i++) { // we need 9 bytes
    data[i] = ds.read();
  }

  ds.reset_search();

  byte MSB = data[1];
  byte LSB = data[0];

  float tempRead = ((MSB << 8) | LSB); //using two's compliment
  float TemperatureSum = tempRead / 16;

  return TemperatureSum;
 

  
}
