// Define pins
const int relay1Pin = 2;
const int relay2Pin = 3;
const int relay3Pin = 14;
const int relay4Pin = 15;
const int voltageSensor1Pin = A0;
const int voltageSensor2Pin = A1;
const int voltageSensor3Pin = A2;
const int voltageSensor4Pin = A3;
const int led1Pin = 4;
const int led2Pin = 5;
const int led3Pin = 6;
const int led4Pin = 7;
const int led5Pin = 8;
const int led6Pin = 9;

// Threshold values
const float voltageThreshold = 6.0; // in Volts
const float voltageSwitchThreshold = 3.5; // in Volts
const float lowVoltageThreshold = 2.5; // in Volts

void setup() {
  // Initialize serial communication
  Serial.begin(9600);

  // Initialize relay pins as outputs
  pinMode(relay1Pin, OUTPUT);
  pinMode(relay2Pin, OUTPUT);
  pinMode(relay3Pin, OUTPUT);
  pinMode(relay4Pin, OUTPUT);

  // Initialize LED pins as outputs
  pinMode(led1Pin, OUTPUT);
  pinMode(led2Pin, OUTPUT);
  pinMode(led3Pin, OUTPUT);
  pinMode(led4Pin, OUTPUT);
  pinMode(led5Pin, OUTPUT);
  pinMode(led6Pin, OUTPUT);

  // Start with Battery-1
  digitalWrite(relay1Pin, LOW); // Battery-1 connected to motor
  digitalWrite(relay2Pin, HIGH); // No charging initially
  digitalWrite(relay3Pin, HIGH); // No EB supply initially
  digitalWrite(relay4Pin, HIGH); // No low voltage alert initially
  digitalWrite(led1Pin, HIGH); // LED-1 ON (Battery-1 in use)
  digitalWrite(led2Pin, LOW); // LED-2 OFF
  digitalWrite(led3Pin, LOW); // LED-3 OFF
  digitalWrite(led4Pin, LOW); // LED-4 OFF
  digitalWrite(led5Pin, LOW); // LED-5 OFF
  digitalWrite(led6Pin, LOW); // LED-6 OFF
}

void loop() {
  // Read voltage values
  float voltage1 = analogRead(voltageSensor1Pin) * (5.0 / 1023.0) * 5; // Assuming a voltage divider
  float voltage2 = analogRead(voltageSensor2Pin) * (5.0 / 1023.0) * 5; // Assuming a voltage divider
  float voltage3 = analogRead(voltageSensor3Pin) * (5.0 / 1023.0) * 11; // Assuming a voltage divider
  float voltage4 = analogRead(voltageSensor4Pin) * (5.0 / 1023.0) * 5; // Assuming a voltage divider

  // Print values for debugging
  Serial.print("Voltage1: "); Serial.println(voltage1);
  Serial.print("Voltage2: "); Serial.println(voltage2);
  Serial.print("Voltage3: "); Serial.println(voltage3);
  Serial.print("Voltage4: "); Serial.println(voltage4);
  Serial.println("===============");

  // Check if Battery-1 needs to be switched
  if (voltage1 <= voltageSwitchThreshold) {
    digitalWrite(relay1Pin, HIGH); // Switch to Battery-2
    digitalWrite(relay2Pin, LOW); // Start charging Battery-1
    digitalWrite(led1Pin, LOW); // LED-1 OFF
    digitalWrite(led2Pin, HIGH); // LED-2 ON (Battery-2 in use)
    digitalWrite(led3Pin, HIGH); // LED-3 ON (Battery-1 charging)
    digitalWrite(led4Pin, LOW); // LED-4 OFF
  }

  // Check if Battery-2 needs to be switched
  if (voltage2 <= voltageSwitchThreshold) {
    digitalWrite(relay1Pin, LOW); // Switch to Battery-1
    digitalWrite(relay2Pin, LOW); // Start charging Battery-2
    digitalWrite(led1Pin, HIGH); // LED-1 ON (Battery-1 in use)
    digitalWrite(led2Pin, LOW); // LED-2 OFF
    digitalWrite(led3Pin, LOW); // LED-3 OFF
    digitalWrite(led4Pin, HIGH); // LED-4 ON (Battery-2 charging)
  }

  // Stop charging Battery-1 if fully charged
  if (voltage1 >= voltageThreshold) {
    digitalWrite(relay2Pin, HIGH);
    digitalWrite(led3Pin, LOW); // LED-3 OFF (Battery-1 fully charged)
  }

  // Stop charging Battery-2 if fully charged
  if (voltage2 >= voltageThreshold) {
    digitalWrite(relay2Pin, HIGH);
    digitalWrite(led4Pin, LOW); // LED-4 OFF (Battery-2 fully charged)
  }

  // Check if solar panel voltage is zero
  if (voltage3 == 0) {
    digitalWrite(relay3Pin, LOW); // Use EB supply to charge batteries
    digitalWrite(led5Pin, LOW); // LED-5 OFF (Solar OFF)
    digitalWrite(led6Pin, HIGH); // LED-6 ON (EB supply ON)
  } else {
    digitalWrite(relay3Pin, HIGH); // Use solar panel to charge batteries
    digitalWrite(led5Pin, HIGH); // LED-5 ON (Solar ON)
    digitalWrite(led6Pin, LOW); // LED-6 OFF (EB supply OFF)
  }

  // Check if EB supply voltage is zero
  if (voltage4 == 0) {
    digitalWrite(relay2Pin, LOW); // Use EB supply to charge batteries
    digitalWrite(led6Pin, HIGH); // LED-6 ON (EB supply ON)
  } else {
    digitalWrite(relay2Pin, HIGH); // Use solar panel to charge batteries
    digitalWrite(led6Pin, LOW); // LED-6 OFF (EB supply OFF)
  }

  // Check for low voltage on Battery-1
  if (voltage1 <= lowVoltageThreshold) {
    digitalWrite(relay4Pin, LOW); // Alert for low voltage on Battery-1
  } else {
    digitalWrite(relay4Pin, HIGH);
  }

  // Check for low voltage on Battery-2
  if (voltage2 <= lowVoltageThreshold) {
    digitalWrite(relay4Pin, HIGH); // Alert for low voltage on Battery-2
  } else {
    digitalWrite(relay4Pin, LOW);
  }

  delay(1000); // Delay for stability
}
