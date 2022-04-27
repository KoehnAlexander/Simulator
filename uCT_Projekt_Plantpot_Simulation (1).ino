/*  Name: Plantpot simulation
 *  Description: Simulates the moisture increase of the soil via waterpump
 *  
 *  Programmers: A. Koehn, M. Riess, T. Grohe and U. Neumann
 *  Feature: Emergency stop before flooding
 *  Version: 1.1.3 28.02.2022 17:58
 */

//-----Defines:-----
#define SIMULATION_PERIOD 2000

//-----Pins:-----
#define LED_HIGH 255
#define LED_LOW 0
#define SOILMOISTURE_PIN 9 // pin where arduino sends the moisture information
#define PUMPCOMMAND_PIN 6  // pin where arduino gets the command to activate pump
#define PUMPLED_PIN 5// pin where arduino sets an active pump LED
#define RED_LED 7
#define YELLOW_LED_2 2
#define YELLOW_LED_1 3
#define GREEN_LED 4

//-----Variables:-----
bool pump_command = 0; // value is HIGH or LOW
int soil_moisture = 0; // value goes from 0 dry to 255 wet
bool error_flag = false;

void setup() {
  Serial.begin(9600);
  setPwmFrequency(9,1);
  pinMode(SOILMOISTURE_PIN, OUTPUT);
  pinMode(PUMPCOMMAND_PIN, INPUT);
  pinMode(PUMPLED_PIN, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(YELLOW_LED_1, OUTPUT);
  pinMode(YELLOW_LED_2, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
}

void loop() {
  if(soil_moisture) soil_moisture--;
  pump_command = digitalRead(PUMPCOMMAND_PIN);
  Serial.print(pump_command*50);
  Serial.print(" ");
  if(pump_command && !error_flag){
    soil_moisture += 10; // soil gets wet
    analogWrite(PUMPLED_PIN, LED_HIGH); // led HIGH
  }
  else{
    analogWrite(PUMPLED_PIN, LED_LOW);
  }

  //Checking error_flag:
  if(soil_moisture < 240){
    error_flag = false;
  }
  else if(soil_moisture >= 240){ //Warning and emergency stop of pump
    error_flag = true;
  }

  if (error_flag){
    analogWrite(PUMPLED_PIN, LED_HIGH);
  }
  delay(SIMULATION_PERIOD/2);
  analogWrite(SOILMOISTURE_PIN, soil_moisture);
    Serial.println(soil_moisture);
  if (error_flag){
    analogWrite(PUMPLED_PIN, LED_LOW);
  }
  delay(SIMULATION_PERIOD/2);
  refreshSoilmeter();
}

void refreshSoilmeter(){
  analogWrite(GREEN_LED, LED_LOW);
  analogWrite(YELLOW_LED_1, LED_LOW);
  analogWrite(YELLOW_LED_2, LED_LOW);
  analogWrite(RED_LED, LED_HIGH);
  if (soil_moisture > 70){
    analogWrite(YELLOW_LED_2, LED_HIGH);
    if(soil_moisture > 130){
      analogWrite(YELLOW_LED_1, LED_HIGH);
      if(soil_moisture > 190){
        analogWrite(GREEN_LED, LED_HIGH);
      }
    }
  }
}
/**
 * https://www.arduino.cc/en/Tutorial/SecretsOfArduinoPWM
 * Divides a given PWM pin frequency by a divisor.
 * 
 * The resulting frequency is equal to the base frequency divided by
 * the given divisor:
 *   - Base frequencies:
 *      o The base frequency for pins 3, 9, 10, and 11 is 31250 Hz.
 *      o The base frequency for pins 5 and 6 is 62500 Hz.
 *   - Divisors:
 *      o The divisors available on pins 5, 6, 9 and 10 are: 1, 8, 64,
 *        256, and 1024.
 *      o The divisors available on pins 3 and 11 are: 1, 8, 32, 64,
 *        128, 256, and 1024.
 * 
 * PWM frequencies are tied together in pairs of pins. If one in a
 * pair is changed, the other is also changed to match:
 *   - Pins 5 and 6 are paired on timer0
 *   - Pins 9 and 10 are paired on timer1
 *   - Pins 3 and 11 are paired on timer2
 * 
 * Note that this function will have side effects on anything else
 * that uses timers:
 *   - Changes on pins 3, 5, 6, or 11 may cause the delay() and
 *     millis() functions to stop working. Other timing-related
 *     functions may also be affected.
 *   - Changes on pins 9 or 10 will cause the Servo library to function
 *     incorrectly.
 * 
 * Thanks to macegr of the Arduino forums for his documentation of the
 * PWM frequency divisors. His post can be viewed at:
 *   http://forum.arduino.cc/index.php?topic=16612#msg121031
 */
void setPwmFrequency(int pin, int divisor) {
  byte mode;
  if(pin == 5 || pin == 6 || pin == 9 || pin == 10) {
    switch(divisor) {
      case 1: mode = 0x01; break;
      case 8: mode = 0x02; break;
      case 64: mode = 0x03; break;
      case 256: mode = 0x04; break;
      case 1024: mode = 0x05; break;
      default: return;
    }
    if(pin == 5 || pin == 6) {
      TCCR0B = TCCR0B & 0b11111000 | mode;
    } else {
      TCCR1B = TCCR1B & 0b11111000 | mode;
    }
  } else if(pin == 3 || pin == 11) {
    switch(divisor) {
      case 1: mode = 0x01; break;
      case 8: mode = 0x02; break;
      case 32: mode = 0x03; break;
      case 64: mode = 0x04; break;
      case 128: mode = 0x05; break;
      case 256: mode = 0x06; break;
      case 1024: mode = 0x07; break;
      default: return;
    }
    TCCR2B = TCCR2B & 0b11111000 | mode;
  }
}
