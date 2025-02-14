// Pin Definitions
#define RELAY_BATTERY_SWITCH 2  // Relay-1 (Battery-1 <-> Battery-2 Switching)
#define RELAY_SOLAR 3           // Relay-2 (Solar Panel Charging Control)
#define RELAY_EB 4              // Relay-3 (EB Supply Charging Control)
#define RELAY_BATTERY_SELECT 5  // Relay-4 (Battery Selection for Charging)

#define LED_BATTERY1 6  // LED-1 (Battery-1 in use)
#define LED_BATTERY2 7  // LED-2 (Battery-2 in use)
#define LED_CHARGING1 8 // LED-3 (Battery-1 charging)
#define LED_CHARGING2 9 // LED-4 (Battery-2 charging)
#define LED_SOLAR 10    // LED-5 (Solar Charging Indicator)
#define LED_EB 11       // LED-6 (EB Charging Indicator)

#define VOLT_SENSOR_BAT1 A0
#define VOLT_SENSOR_BAT2 A1
#define VOLT_SENSOR_SOLAR A2
#define VOLT_SENSOR_EB A3

// Battery Charge Thresholds
#define BATTERY_FULL 90  // 90% charge
#define BATTERY_LOW 30   // 30% charge
#define SOLAR_THRESHOLD 20 // 20-25V for solar charging
#define EB_THRESHOLD 64   // EB Supply threshold

void setup() {
    Serial.begin(9600);
    pinMode(RELAY_BATTERY_SWITCH, OUTPUT);
    pinMode(RELAY_SOLAR, OUTPUT);
    pinMode(RELAY_EB, OUTPUT);
    pinMode(RELAY_BATTERY_SELECT, OUTPUT);
    pinMode(LED_BATTERY1, OUTPUT);
    pinMode(LED_BATTERY2, OUTPUT);
    pinMode(LED_CHARGING1, OUTPUT);
    pinMode(LED_CHARGING2, OUTPUT);
    pinMode(LED_SOLAR, OUTPUT);
    pinMode(LED_EB, OUTPUT);
    
    digitalWrite(RELAY_BATTERY_SWITCH, LOW);
    digitalWrite(RELAY_SOLAR, LOW);
    digitalWrite(RELAY_EB, LOW);
    digitalWrite(RELAY_BATTERY_SELECT, LOW);
}

void loop() {
    int bat1Voltage = analogRead(VOLT_SENSOR_BAT1) * (5.0 / 1023.0) * 11;
    int bat2Voltage = analogRead(VOLT_SENSOR_BAT2) * (5.0 / 1023.0) * 11;
    int solarVoltage = analogRead(VOLT_SENSOR_SOLAR) * (5.0 / 1023.0) * 11;
    int ebVoltage = analogRead(VOLT_SENSOR_EB) * (5.0 / 1023.0) * 11;
    
    Serial.print("Battery-1 Voltage: "); Serial.println(bat1Voltage);
    Serial.print("Battery-2 Voltage: "); Serial.println(bat2Voltage);
    Serial.print("Solar Voltage: "); Serial.println(solarVoltage);
    Serial.print("EB Voltage: "); Serial.println(ebVoltage);
    
    if (bat1Voltage > BATTERY_LOW) {
        digitalWrite(RELAY_BATTERY_SWITCH, LOW); // Battery-1 in use
        digitalWrite(LED_BATTERY1, HIGH);
        digitalWrite(LED_BATTERY2, LOW);
    } else {
        digitalWrite(RELAY_BATTERY_SWITCH, HIGH); // Switch to Battery-2
        digitalWrite(LED_BATTERY1, LOW);
        digitalWrite(LED_BATTERY2, HIGH);
    }
    
    if (bat1Voltage < BATTERY_FULL && bat2Voltage < BATTERY_FULL) {
        if (solarVoltage >= SOLAR_THRESHOLD) {
            digitalWrite(RELAY_SOLAR, HIGH);
            digitalWrite(RELAY_EB, LOW);
            digitalWrite(LED_SOLAR, HIGH);
            digitalWrite(LED_EB, LOW);
        } else if (ebVoltage >= EB_THRESHOLD) {
            digitalWrite(RELAY_SOLAR, LOW);
            digitalWrite(RELAY_EB, HIGH);
            digitalWrite(LED_SOLAR, LOW);
            digitalWrite(LED_EB, HIGH);
        } else {
            digitalWrite(RELAY_SOLAR, LOW);
            digitalWrite(RELAY_EB, LOW);
            digitalWrite(LED_SOLAR, LOW);
            digitalWrite(LED_EB, LOW);
        }
    }
    
    if (bat1Voltage < BATTERY_FULL) {
        digitalWrite(RELAY_BATTERY_SELECT, LOW); // Charge Battery-1
        digitalWrite(LED_CHARGING1, HIGH);
        digitalWrite(LED_CHARGING2, LOW);
    } else if (bat2Voltage < BATTERY_FULL) {
        digitalWrite(RELAY_BATTERY_SELECT, HIGH); // Charge Battery-2
        digitalWrite(LED_CHARGING1, LOW);
        digitalWrite(LED_CHARGING2, HIGH);
    } else {
        digitalWrite(LED_CHARGING1, LOW);
        digitalWrite(LED_CHARGING2, LOW);
    }
    
    // Send Data Over Serial for Dashboard
    String data = String(bat1Voltage) + "," + String(bat2Voltage) + "," +
                  String(solarVoltage) + "," + String(ebVoltage) + "," +
                  String(digitalRead(RELAY_BATTERY_SWITCH)) + "," +
                  String(digitalRead(RELAY_SOLAR)) + "," +
                  String(digitalRead(RELAY_EB)) + "," +
                  String(digitalRead(RELAY_BATTERY_SELECT));
    Serial.println(data);
    
    delay(2000);
}
