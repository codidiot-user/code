// Define pins
#define RELAY1_PIN 2 // Battery 1/2 selection
#define RELAY2_PIN 3 // Solar panel connection
#define RELAY3_PIN 4 // EB connection
#define RELAY4_PIN 5 // Battery charging selection (1/2)

#define VOLTAGE1_PIN A0 // Battery 1 voltage sensor
#define VOLTAGE2_PIN A1 // Battery 2 voltage sensor
#define SOLAR_VOLTAGE_PIN A2 // Solar panel voltage sensor
#define EB_VOLTAGE_PIN A3 // EB voltage sensor

#define LED1_PIN 6 // Battery 1 in use
#define LED2_PIN 7 // Battery 2 in use
#define LED3_PIN 8 // Battery 1 charging
#define LED4_PIN 9 // Battery 2 charging
#define LED5_PIN 10 // Solar charging
#define LED6_PIN 11 // EB charging

// Define thresholds
#define BATTERY_FULL 90 // Percentage
#define BATTERY_LOW 30 // Percentage
#define SOLAR_THRESHOLD 20 // Volts
#define EB_THRESHOLD 64 // Volts

// Variables
float battery1Voltage, battery2Voltage, solarVoltage, ebVoltage;
int battery1Percentage, battery2Percentage;
bool vehicleStandstill = true; // Replace with actual standstill detection

void setup() {
  Serial.begin(9600);

  // Set pin modes
  pinMode(RELAY1_PIN, OUTPUT);
  pinMode(RELAY2_PIN, OUTPUT);
  pinMode(RELAY3_PIN, OUTPUT);
  pinMode(RELAY4_PIN, OUTPUT);

  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  pinMode(LED3_PIN, OUTPUT);
  pinMode(LED4_PIN, OUTPUT);
  pinMode(LED5_PIN, OUTPUT);
  pinMode(LED6_PIN, OUTPUT);


  // Initialize relays (Battery 1 connected initially)
  digitalWrite(RELAY1_PIN, LOW); // NC for Battery 1
  digitalWrite(RELAY4_PIN, HIGH); // NO for Battery 1 charging initially
  digitalWrite(RELAY2_PIN, LOW);  // Solar panel disconnected
  digitalWrite(RELAY3_PIN, LOW); // EB disconnected

}

void loop() {
  // Read voltages
  battery1Voltage = readVoltage(VOLTAGE1_PIN);
  battery2Voltage = readVoltage(VOLTAGE2_PIN);
  solarVoltage = readVoltage(SOLAR_VOLTAGE_PIN);
  ebVoltage = readVoltage(EB_VOLTAGE_PIN);

  // Calculate battery percentages
  battery1Percentage = map(battery1Voltage, 0, 12, 0, 100); // Adjust 12V to your max voltage
  battery2Percentage = map(battery2Voltage, 0, 12, 0, 100); // Adjust 12V to your max voltage

  // Serial Monitor Output
  Serial.print("Battery 1: "); Serial.print(battery1Percentage); Serial.print("% ("); Serial.print(battery1Voltage); Serial.print("V), ");
  Serial.print("Battery 2: "); Serial.print(battery2Percentage); Serial.print("% ("); Serial.print(battery2Voltage); Serial.print("V), ");
  Serial.print("Solar: "); Serial.print(solarVoltage); Serial.print("V, ");
  Serial.print("EB: "); Serial.print(ebVoltage); Serial.println("V");


  // Battery Management Logic
  if (battery1Percentage >= BATTERY_FULL && battery2Percentage >= BATTERY_FULL) {
    // Both full, do nothing (or turn off all charging)
    digitalWrite(RELAY2_PIN, LOW); // Disconnect Solar
    digitalWrite(RELAY3_PIN, LOW); // Disconnect EB
    digitalWrite(LED3_PIN, LOW);
    digitalWrite(LED4_PIN, LOW);
    digitalWrite(LED5_PIN, LOW);
    digitalWrite(LED6_PIN, LOW);

  } else if (battery1Percentage <= BATTERY_LOW) {
    // Switch to Battery 2
    digitalWrite(RELAY1_PIN, HIGH); // NC for Battery 2
    digitalWrite(LED1_PIN, LOW);
    digitalWrite(LED2_PIN, HIGH);

    // Charge Battery 1 (Solar or EB)
    chargeBattery(1);
  } else {
    // Battery 1 in use
    digitalWrite(RELAY1_PIN, LOW); // NC for Battery 1
    digitalWrite(LED1_PIN, HIGH);
    digitalWrite(LED2_PIN, LOW);

    // Charge Battery 2 (Solar or EB)
    chargeBattery(2);
  }

  delay(1000); // Delay for reading stability
}

// Function to charge a battery (1 or 2)
void chargeBattery(int battery) {
  if (battery == 1) {
    digitalWrite(RELAY4_PIN, HIGH); // NO for Battery 1 charging
    digitalWrite(LED3_PIN, HIGH);  //LED for battery 1 charging
    digitalWrite(LED4_PIN, LOW);
  } else {
    digitalWrite(RELAY4_PIN, LOW); // NC for Battery 2 charging
    digitalWrite(LED3_PIN, LOW);
    digitalWrite(LED4_PIN, HIGH); //LED for battery 2 charging
  }

  if (solarVoltage >= SOLAR_THRESHOLD) {
    digitalWrite(RELAY2_PIN, HIGH); // Connect Solar
    digitalWrite(RELAY3_PIN, LOW);  // Disconnect EB
    digitalWrite(LED5_PIN, HIGH);   // Solar LED
    digitalWrite(LED6_PIN, LOW);
  } else if (ebVoltage >= EB_THRESHOLD && vehicleStandstill) {
    digitalWrite(RELAY2_PIN, LOW);  // Disconnect Solar
    digitalWrite(RELAY3_PIN, HIGH); // Connect EB
    digitalWrite(LED5_PIN, LOW);
    digitalWrite(LED6_PIN, HIGH); // EB LED
  } else {
    digitalWrite(RELAY2_PIN, LOW);  // Disconnect Solar
    digitalWrite(RELAY3_PIN, LOW); // Disconnect EB
    digitalWrite(LED5_PIN, LOW);
    digitalWrite(LED6_PIN, LOW);
  }
}

// Function to read voltage and convert to volts
float readVoltage(int pin) {
  int rawValue = analogRead(pin);
  float voltage = rawValue * (5.0 / 1023.0); // Assuming 5V reference, adjust if needed.
  return voltage;
}
