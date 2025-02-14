// Pin Definitions
const int relay1 = 22;  // Relay 1: Battery-1 (NO) and Battery-2 (NC)
const int relay2 = 24;  // Relay 2: Solar Panel (NO)
const int relay3 = 26;  // Relay 3: EB Supply (NO)
const int relay4 = 28;  // Relay 4: Charging Battery-1 (NO) or Battery-2 (NC)

const int led1 = 30;    // LED-1: Battery-1 in use
const int led2 = 32;    // LED-2: Battery-2 in use
const int led3 = 34;    // LED-3: Battery-1 charging
const int led4 = 36;    // LED-4: Battery-2 charging
const int led5 = 38;    // LED-5: Solar charging
const int led6 = 40;    // LED-6: EB charging

const int voltageSensor1 = A0;  // Voltage sensor for Battery-1
const int voltageSensor2 = A1;  // Voltage sensor for Battery-2
const int voltageSensorSolar = A2;  // Voltage sensor for Solar Panel
const int voltageSensorEB = A3;  // Voltage sensor for EB Supply

// Thresholds
const float BATTERY_LOW = 30.0;  // 30% charge
const float BATTERY_HIGH = 90.0; // 90% charge
const float SOLAR_VOLTAGE_MIN = 20.0;  // Minimum solar voltage for charging
const float SOLAR_VOLTAGE_MAX = 25.0;  // Maximum solar voltage for charging
const float EB_VOLTAGE_MIN = 64.0;  // Minimum EB voltage for charging

// Variables
float battery1Voltage = 0.0;
float battery2Voltage = 0.0;
float solarVoltage = 0.0;
float ebVoltage = 0.0;

bool isVehicleStandstill = true;  // Assume vehicle is standstill for now

void setup() {
  // Initialize Serial Monitor
  Serial.begin(9600);

  // Set pin modes
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  pinMode(relay3, OUTPUT);
  pinMode(relay4, OUTPUT);

  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(led4, OUTPUT);
  pinMode(led5, OUTPUT);
  pinMode(led6, OUTPUT);

  // Initialize relays to default state
  digitalWrite(relay1, HIGH);  // Battery-1 connected to motor
  digitalWrite(relay2, LOW);   // Solar panel disconnected
  digitalWrite(relay3, LOW);   // EB supply disconnected
  digitalWrite(relay4, HIGH);  // Battery-1 charging (NO)

  // Initialize LEDs
  digitalWrite(led1, HIGH);  // Battery-1 in use
  digitalWrite(led2, LOW);
  digitalWrite(led3, LOW);
  digitalWrite(led4, LOW);
  digitalWrite(led5, LOW);
  digitalWrite(led6, LOW);
}

void loop() {
  // Read voltages
  battery1Voltage = readVoltage(voltageSensor1);
  battery2Voltage = readVoltage(voltageSensor2);
  solarVoltage = readVoltage(voltageSensorSolar);
  ebVoltage = readVoltage(voltageSensorEB);

  // Print values to Serial Monitor
  Serial.print("Battery-1 Voltage: ");
  Serial.println(battery1Voltage);
  Serial.print("Battery-2 Voltage: ");
  Serial.println(battery2Voltage);
  Serial.print("Solar Voltage: ");
  Serial.println(solarVoltage);
  Serial.print("EB Voltage: ");
  Serial.println(ebVoltage);
  Serial.println("-----------------------------");

  // Check battery levels and switch between them
  if (battery1Voltage <= BATTERY_LOW) {
    switchToBattery2();
  } else if (battery2Voltage <= BATTERY_LOW) {
    switchToBattery1();
  }

  // Charging logic
  if (battery1Voltage < BATTERY_HIGH || battery2Voltage < BATTERY_HIGH) {
    if (solarVoltage >= SOLAR_VOLTAGE_MIN && solarVoltage <= SOLAR_VOLTAGE_MAX) {
      chargeWithSolar();
    } else if (ebVoltage >= EB_VOLTAGE_MIN && isVehicleStandstill) {
      chargeWithEB();
    }
  }

  delay(1000);  // Delay for stability
}

float readVoltage(int pin) {
  int sensorValue = analogRead(pin);
  float voltage = sensorValue * (5.0 / 1023.0);  // Convert analog reading to voltage
  return voltage;
}

void switchToBattery1() {
  digitalWrite(relay1, HIGH);  // Connect Battery-1 to motor
  digitalWrite(led1, HIGH);     // LED-1 ON
  digitalWrite(led2, LOW);      // LED-2 OFF
}

void switchToBattery2() {
  digitalWrite(relay1, LOW);   // Connect Battery-2 to motor
  digitalWrite(led1, LOW);     // LED-1 OFF
  digitalWrite(led2, HIGH);    // LED-2 ON
}

void chargeWithSolar() {
  digitalWrite(relay2, HIGH);  // Connect Solar Panel
  digitalWrite(relay3, LOW);   // Disconnect EB Supply
  digitalWrite(relay4, HIGH);   // Charge Battery-1 (NO)
  digitalWrite(led5, HIGH);     // LED-5 ON
  digitalWrite(led6, LOW);      // LED-6 OFF
}

void chargeWithEB() {
  digitalWrite(relay2, LOW);    // Disconnect Solar Panel
  digitalWrite(relay3, HIGH);   // Connect EB Supply
  digitalWrite(relay4, LOW);    // Charge Battery-2 (NC)
  digitalWrite(led5, LOW);      // LED-5 OFF
  digitalWrite(led6, HIGH);     // LED-6 ON
}
