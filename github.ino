// Pin definitions
// Voltage Sensors
const int BATTERY1_VOLTAGE_PIN = A0;  // Voltage sensor for Battery 1
const int BATTERY2_VOLTAGE_PIN = A1;  // Voltage sensor for Battery 2
const int SOLAR_VOLTAGE_PIN = A2;     // Voltage sensor for Solar Panel
const int EB_VOLTAGE_PIN = A3;        // Voltage sensor for EB Supply

// Relay Module Pins
const int RELAY1_PIN = 2;  // Battery switching relay (NO: Battery1, NC: Battery2)
const int RELAY2_PIN = 3;  // Solar Panel relay (NO)
const int RELAY3_PIN = 4;  // EB Supply relay (NO)
const int RELAY4_PIN = 5;  // Charging selection relay (NO: Battery1, NC: Battery2)

// LED Pins
const int LED1_PIN = 6;   // Battery 1 in use
const int LED2_PIN = 7;   // Battery 2 in use
const int LED3_PIN = 8;   // Battery 1 charging
const int LED4_PIN = 9;   // Battery 2 charging
const int LED5_PIN = 10;  // Solar charging indicator
const int LED6_PIN = 11;  // EB charging indicator

// Constants
const float BATTERY_MAX_VOLTAGE = 12.6;  // 100% charge voltage
const float BATTERY_MIN_VOLTAGE = 10.8;  // 0% charge voltage
const int SOLAR_MIN_VOLTAGE = 20;        // Minimum solar voltage for charging
const int EB_MIN_VOLTAGE = 64;           // Minimum EB voltage for charging
const int CHARGE_MAX_THRESHOLD = 90;     // Maximum charge percentage
const int CHARGE_MIN_THRESHOLD = 30;     // Minimum charge percentage

// Variables
float battery1Voltage = 0;
float battery2Voltage = 0;
float solarVoltage = 0;
float ebVoltage = 0;
int battery1Percentage = 0;
int battery2Percentage = 0;
bool isVehicleStandstill = true;  // This should be connected to a real sensor

void setup() {
  Serial.begin(9600);
  
  // Initialize relay pins as outputs
  pinMode(RELAY1_PIN, OUTPUT);
  pinMode(RELAY2_PIN, OUTPUT);
  pinMode(RELAY3_PIN, OUTPUT);
  pinMode(RELAY4_PIN, OUTPUT);
  
  // Initialize LED pins as outputs
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  pinMode(LED3_PIN, OUTPUT);
  pinMode(LED4_PIN, OUTPUT);
  pinMode(LED5_PIN, OUTPUT);
  pinMode(LED6_PIN, OUTPUT);
  
  // Initially all relays OFF
  digitalWrite(RELAY1_PIN, LOW);
  digitalWrite(RELAY2_PIN, LOW);
  digitalWrite(RELAY3_PIN, LOW);
  digitalWrite(RELAY4_PIN, LOW);
}

void loop() {
  // Read voltage values
  readVoltages();
  
  // Calculate battery percentages
  calculateBatteryPercentages();
  
  // Print status to serial monitor
  printStatus();
  
  // Main battery management logic
  manageBatteries();
  
  delay(1000);  // Update every second
}

void readVoltages() {
  // Convert analog readings to voltage (adjust multiplier based on voltage divider)
  battery1Voltage = analogRead(BATTERY1_VOLTAGE_PIN) * (5.0 / 1023.0) * 4;  // Assuming 4:1 voltage divider
  battery2Voltage = analogRead(BATTERY2_VOLTAGE_PIN) * (5.0 / 1023.0) * 4;
  solarVoltage = analogRead(SOLAR_VOLTAGE_PIN) * (5.0 / 1023.0) * 10;      // Assuming 10:1 voltage divider
  ebVoltage = analogRead(EB_VOLTAGE_PIN) * (5.0 / 1023.0) * 20;            // Assuming 20:1 voltage divider
}

void calculateBatteryPercentages() {
  // Convert voltage to percentage
  battery1Percentage = map(battery1Voltage * 100, BATTERY_MIN_VOLTAGE * 100, BATTERY_MAX_VOLTAGE * 100, 0, 100);
  battery2Percentage = map(battery2Voltage * 100, BATTERY_MIN_VOLTAGE * 100, BATTERY_MAX_VOLTAGE * 100, 0, 100);
  
  // Constrain values between 0 and 100
  battery1Percentage = constrain(battery1Percentage, 0, 100);
  battery2Percentage = constrain(battery2Percentage, 0, 100);
}

void manageBatteries() {
  static bool battery1Active = true;  // Start with battery 1
  
  // Battery switching logic
  if (battery1Active && battery1Percentage <= CHARGE_MIN_THRESHOLD) {
    // Switch to battery 2
    battery1Active = false;
    digitalWrite(RELAY1_PIN, HIGH);  // Switch to Battery 2 (NC)
    digitalWrite(LED1_PIN, LOW);
    digitalWrite(LED2_PIN, HIGH);
  } else if (!battery1Active && battery2Percentage <= CHARGE_MIN_THRESHOLD) {
    // Switch to battery 1
    battery1Active = true;
    digitalWrite(RELAY1_PIN, LOW);   // Switch to Battery 1 (NO)
    digitalWrite(LED1_PIN, HIGH);
    digitalWrite(LED2_PIN, LOW);
  }
  
  // Charging logic
  if (battery1Percentage < CHARGE_MAX_THRESHOLD && !battery1Active) {
    chargeBattery(1);
  } else if (battery2Percentage < CHARGE_MAX_THRESHOLD && battery1Active) {
    chargeBattery(2);
  }
}

void chargeBattery(int batteryNumber) {
  bool usingSolar = false;
  
  // Check if solar charging is possible
  if (solarVoltage >= SOLAR_MIN_VOLTAGE) {
    usingSolar = true;
    digitalWrite(RELAY2_PIN, HIGH);  // Enable solar charging
    digitalWrite(RELAY3_PIN, LOW);   // Disable EB charging
    digitalWrite(LED5_PIN, HIGH);
    digitalWrite(LED6_PIN, LOW);
  }
  // Check if EB charging is possible
  else if (ebVoltage >= EB_MIN_VOLTAGE && isVehicleStandstill) {
    usingSolar = false;
    digitalWrite(RELAY2_PIN, LOW);   // Disable solar charging
    digitalWrite(RELAY3_PIN, HIGH);  // Enable EB charging
    digitalWrite(LED5_PIN, LOW);
    digitalWrite(LED6_PIN, HIGH);
  } else {
    // No charging source available
    digitalWrite(RELAY2_PIN, LOW);
    digitalWrite(RELAY3_PIN, LOW);
    digitalWrite(LED5_PIN, LOW);
    digitalWrite(LED6_PIN, LOW);
    return;
  }
  
  // Set charging battery selection relay
  if (batteryNumber == 1) {
    digitalWrite(RELAY4_PIN, LOW);   // Charge Battery 1 (NO)
    digitalWrite(LED3_PIN, HIGH);
    digitalWrite(LED4_PIN, LOW);
  } else {
    digitalWrite(RELAY4_PIN, HIGH);  // Charge Battery 2 (NC)
    digitalWrite(LED3_PIN, LOW);
    digitalWrite(LED4_PIN, HIGH);
  }
}

void printStatus() {
  Serial.println("=== System Status ===");
  Serial.print("Battery 1: ");
  Serial.print(battery1Voltage);
  Serial.print("V (");
  Serial.print(battery1Percentage);
  Serial.println("%)");
  
  Serial.print("Battery 2: ");
  Serial.print(battery2Voltage);
  Serial.print("V (");
  Serial.print(battery2Percentage);
  Serial.println("%)");
  
  Serial.print("Solar Panel: ");
  Serial.print(solarVoltage);
  Serial.println("V");
  
  Serial.print("EB Supply: ");
  Serial.print(ebVoltage);
  Serial.println("V");
  
  Serial.println("==================\n");
}
