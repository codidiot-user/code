#include <Wire.h>

// Constants
const int battery1Pin = A0; // Voltage sensor for Battery 1
const int battery2Pin = A1; // Voltage sensor for Battery 2
const int solarPanelPin = A2; // Voltage sensor for Solar Panel
const int ebSupplyPin = A3; // Voltage sensor for EB Supply

const int relayBattery1Pin = 2;
const int relayBattery2Pin = 3;
const int relaySolarPin = 4;
const int relayEbPin = 5;

const int led1Pin = 6; // LED for Battery 1 in use
const int led2Pin = 7; // LED for Battery 2 in use
const int led3Pin = 8; // LED for Battery 1 charging
const int led4Pin = 9; // LED for Battery 2 charging
const int led5Pin = 10; // LED for Solar charging
const int led6Pin = 11; // LED for EB charging

void setup() {
  // Initialize serial communication
  Serial.begin(9600);
  
  // Set pin modes
  pinMode(relayBattery1Pin, OUTPUT);
  pinMode(relayBattery2Pin, OUTPUT);
  pinMode(relaySolarPin, OUTPUT);
  pinMode(relayEbPin, OUTPUT);
  
  pinMode(led1Pin, OUTPUT);
  pinMode(led2Pin, OUTPUT);
  pinMode(led3Pin, OUTPUT);
  pinMode(led4Pin, OUTPUT);
  pinMode(led5Pin, OUTPUT);
  pinMode(led6Pin, OUTPUT);
}

void loop() {
  // Read voltages
  int battery1Voltage = analogRead(battery1Pin);
  int battery2Voltage = analogRead(battery2Pin);
  int solarPanelVoltage = analogRead(solarPanelPin);
  int ebSupplyVoltage = analogRead(ebSupplyPin);
  
  // Convert analog readings to actual voltages (assuming 5V reference and 10-bit ADC)
  float battery1V = (battery1Voltage / 1023.0) * 5.0 * 11; // Voltage divider factor 11 for 50V max
  float battery2V = (battery2Voltage / 1023.0) * 5.0 * 11;
  float solarV = (solarPanelVoltage / 1023.0) * 5.0 * 11;
  float ebV = (ebSupplyVoltage / 1023.0) * 5.0 * 13; // Voltage divider factor 13 for 65V max
  
  // Print values to serial monitor
  Serial.print("Battery 1 Voltage: ");
  Serial.println(battery1V);
  Serial.print("Battery 2 Voltage: ");
  Serial.println(battery2V);
  Serial.print("Solar Panel Voltage: ");
  Serial.println(solarV);
  Serial.print("EB Supply Voltage: ");
  Serial.println(ebV);
  
  // Control relays and LEDs based on conditions
  // (You will need to add your control logic here based on your conditions)
  // Example logic:
  if (battery1V > 3.0) {
    digitalWrite(relayBattery1Pin, HIGH);
    digitalWrite(led1Pin, HIGH);
    digitalWrite(led3Pin, LOW);
  } else if (battery2V > 3.0) {
    digitalWrite(relayBattery2Pin, HIGH);
    digitalWrite(led2Pin, HIGH);
    digitalWrite(led4Pin, LOW);
  }
  
  // Add other conditions based on your requirements
  
  delay(1000); // Delay to reduce serial output frequency
}
