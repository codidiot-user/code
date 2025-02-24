// Define pins
const int relay1Pin = 2;
const int relay2Pin = 3;
const int relay3Pin = 14;
const int relay4Pin = 15;
const int voltageSensor1Pin = A0;
const int voltageSensor2Pin = A1;
const int voltageSensor3Pin = A3;
const int voltageSensor4Pin = A5;

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

  // Start with Battery-1
  digitalWrite(relay1Pin, LOW); // Battery-1 connected to motor
  digitalWrite(relay2Pin, HIGH); // No charging initially
  digitalWrite(relay3Pin, HIGH); // No EB supply initially
  digitalWrite(relay4Pin, HIGH); // No low voltage alert initially
}

void loop() {
  // Read voltage values
  float voltage1 = analogRead(voltageSensor1Pin) * (5.0 / 1023.0) * 5; // Battery-1
  float voltage2 = analogRead(voltageSensor2Pin) * (5.0 / 1023.0) * 5; // Battery-2
  float voltage3 = analogRead(voltageSensor3Pin) * (5.0 / 1023.0) * 11; // Solar Panel
  float voltage4 = analogRead(voltageSensor4Pin) * (5.0 / 1023.0) * 5; // EB Supply

  // Send data in a structured format
  Serial.print(voltage1);
  Serial.print(",");
  Serial.print(voltage2);
  Serial.print(",");
  Serial.print(voltage3);
  Serial.print(",");
  Serial.println(voltage4);

  // Check if Battery-1 needs to be switched
  if (voltage1 <= voltageSwitchThreshold) {
    digitalWrite(relay1Pin, HIGH); // Switch to Battery-2
    digitalWrite(relay2Pin, LOW); // Start charging Battery-1
  }

  // Check if Battery-2 needs to be switched
  if (voltage2 <= voltageSwitchThreshold) {
    digitalWrite(relay1Pin, LOW); // Switch to Battery-1
    digitalWrite(relay2Pin, LOW); // Start charging Battery-2
  }

  // Stop charging Battery-1 if fully charged
  if (voltage1 >= voltageThreshold) {
    digitalWrite(relay2Pin, HIGH);
  }

  // Stop charging Battery-2 if fully charged
  if (voltage2 >= voltageThreshold) {
    digitalWrite(relay2Pin, HIGH);
  }

  // Check if solar panel voltage is zero
  if (voltage3 == 0) {
    digitalWrite(relay3Pin, LOW); // Use EB supply to charge batteries
  } else {
    digitalWrite(relay3Pin, HIGH); // Use solar panel to charge batteries
  }

  // Check if EB supply voltage is zero
  if (voltage4 == 0) {
    digitalWrite(relay2Pin, LOW); // Use EB supply to charge batteries
  } else {
    digitalWrite(relay2Pin, HIGH); // Use solar panel to charge batteries
  }

  // Check for low voltage on Battery-1 or Battery-2
  if (voltage1 <= lowVoltageThreshold || voltage2 <= lowVoltageThreshold) {
    digitalWrite(relay4Pin, LOW); // Alert for low voltage
  } else {
    digitalWrite(relay4Pin, HIGH); // No alert
  }

  delay(1000); // Delay for stability
}
