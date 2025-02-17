// Pin Definitions
const int BATTERY1_RELAY_PIN = 2;  // Relay Module 1 NO for Battery 1
const int BATTERY2_RELAY_PIN = 3;  // Relay Module 1 NC for Battery 2
const int SOLAR_RELAY_PIN = 4;     // Relay Module 2 NO for Solar Panel
const int EB_RELAY_PIN = 5;        // Relay Module 3 NO for EB Supply
const int CHARGE_RELAY_PIN = 6;    // Relay Module 4, NO for Battery 1, NC for Battery 2

// Voltage Sensors
const int VOLT_SENSOR_SOLAR = A0;  // Solar Panel Voltage
const int VOLT_SENSOR_EB = A1;     // EB Supply Voltage
const int VOLT_SENSOR_BAT1 = A2;   // Battery 1 Voltage
const int VOLT_SENSOR_BAT2 = A3;   // Battery 2 Voltage

// LEDs
const int LED_BAT1_IN_USE = 7;  
const int LED_BAT2_IN_USE = 8;  
const int LED_BAT1_CHARGING = 9; 
const int LED_BAT2_CHARGING = 10;
const int LED_SOLAR_CHARGING = 11;
const int LED_EB_CHARGING = 12;

// Constants for voltage thresholds
const float SOLAR_CHARGE_VOLTAGE_MIN = 20.0;  // Minimum voltage for solar charging
const float SOLAR_CHARGE_VOLTAGE_MAX = 25.0;  // Maximum voltage for solar charging
const float EB_CHARGE_VOLTAGE = 64.0;
const float BATTERY_FULL_PERCENTAGE = 90.0;
const float BATTERY_LOW_PERCENTAGE = 30.0;
const float BATTERY_FULL_VOLTAGE = 7.2;  // Assuming 12V battery at 90% for 6.45Ah
const float BATTERY_LOW_VOLTAGE = 3.6;   // 30% of full charge

// Assuming you have a way to detect if the vehicle is at standstill, for now, we'll use a dummy value
const bool VEHICLE_STANDSTILL = true; // Change this based on actual sensor input

void setup() {
  Serial.begin(9600);
  
  pinMode(BATTERY1_RELAY_PIN, OUTPUT);
  pinMode(BATTERY2_RELAY_PIN, OUTPUT);
  pinMode(SOLAR_RELAY_PIN, OUTPUT);
  pinMode(EB_RELAY_PIN, OUTPUT);
  pinMode(CHARGE_RELAY_PIN, OUTPUT);

  for (int i = 7; i <= 12; i++) {  // Set LED pins as outputs
    pinMode(i, OUTPUT);
  }

  // Start with Battery 1 supplying power
  digitalWrite(BATTERY1_RELAY_PIN, HIGH);
  digitalWrite(BATTERY2_RELAY_PIN, LOW);
  digitalWrite(LED_BAT1_IN_USE, HIGH);
}

void loop() {
  float solarVoltage = analogRead(VOLT_SENSOR_SOLAR) * (5.0 / 1023.0) * 10;  // Assuming 10:1 voltage divider
  float ebVoltage = analogRead(VOLT_SENSOR_EB) * (5.0 / 1023.0) * 10;
  float bat1Voltage = analogRead(VOLT_SENSOR_BAT1) * (5.0 / 1023.0) * 10;
  float bat2Voltage = analogRead(VOLT_SENSOR_BAT2) * (5.0 / 1023.0) * 10;

  // Convert voltage to percentage for easier comparison
  float bat1Percent = (bat1Voltage / BATTERY_FULL_VOLTAGE) * 100;
  float bat2Percent = (bat2Voltage / BATTERY_FULL_VOLTAGE) * 100;

  // Serial Monitor Output
  Serial.print("Battery 1: "); Serial.print(bat1Percent); Serial.print("% | ");
  Serial.print("Battery 2: "); Serial.print(bat2Percent); Serial.print("% | ");
  Serial.print("Solar Voltage: "); Serial.print(solarVoltage); Serial.print("V | ");
  Serial.print("EB Voltage: "); Serial.print(ebVoltage); Serial.println("V");

  // Battery Switch Logic
  if (bat1Percent <= BATTERY_LOW_PERCENTAGE) {
    digitalWrite(BATTERY1_RELAY_PIN, LOW);
    digitalWrite(BATTERY2_RELAY_PIN, HIGH);
    digitalWrite(LED_BAT1_IN_USE, LOW);
    digitalWrite(LED_BAT2_IN_USE, HIGH);
    // Battery 1 now needs to charge
    manageBatteryCharging(1, bat1Percent, solarVoltage, ebVoltage);
  } else if (bat2Percent <= BATTERY_LOW_PERCENTAGE) {
    digitalWrite(BATTERY1_RELAY_PIN, HIGH);
    digitalWrite(BATTERY2_RELAY_PIN, LOW);
    digitalWrite(LED_BAT1_IN_USE, HIGH);
    digitalWrite(LED_BAT2_IN_USE, LOW);
    // Battery 2 now needs to charge
    manageBatteryCharging(2, bat2Percent, solarVoltage, ebVoltage);
  }

  delay(5000); // Check every 5 seconds
}

void manageBatteryCharging(int batteryIndex, float batteryPercent, float solarV, float ebV) {
  int chargeRelay = (batteryIndex == 1) ? HIGH : LOW; // NO for Battery 1, NC for Battery 2
  int chargingLED = (batteryIndex == 1) ? LED_BAT1_CHARGING : LED_BAT2_CHARGING;

  if (batteryPercent < BATTERY_FULL_PERCENTAGE) {
    if (solarV >= SOLAR_CHARGE_VOLTAGE_MIN && solarV <= SOLAR_CHARGE_VOLTAGE_MAX) {
      digitalWrite(SOLAR_RELAY_PIN, HIGH);
      digitalWrite(EB_RELAY_PIN, LOW);
      digitalWrite(CHARGE_RELAY_PIN, chargeRelay);
      digitalWrite(LED_SOLAR_CHARGING, HIGH);
      digitalWrite(LED_EB_CHARGING, LOW);
      digitalWrite(chargingLED, HIGH);
    } else if (ebV >= EB_CHARGE_VOLTAGE && VEHICLE_STANDSTILL) {
      digitalWrite(SOLAR_RELAY_PIN, LOW);
      digitalWrite(EB_RELAY_PIN, HIGH);
      digitalWrite(CHARGE_RELAY_PIN, chargeRelay);
      digitalWrite(LED_SOLAR_CHARGING, LOW);
      digitalWrite(LED_EB_CHARGING, HIGH);
      digitalWrite(chargingLED, HIGH);
    } else {
      // No charging source available or conditions not met
      digitalWrite(SOLAR_RELAY_PIN, LOW);
      digitalWrite(EB_RELAY_PIN, LOW);
      digitalWrite(CHARGE_RELAY_PIN, LOW);
      digitalWrite(LED_SOLAR_CHARGING, LOW);
      digitalWrite(LED_EB_CHARGING, LOW);
      digitalWrite(chargingLED, LOW);
    }
  } else {
    // Battery is full, stop charging
    digitalWrite(CHARGE_RELAY_PIN, LOW);
    digitalWrite(chargingLED, LOW);
  }
}
