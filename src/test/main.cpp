#include <Arduino.h>
#include <Wire.h>
#include <VL53L1X.h>

#define SDA_PIN 8
#define SCL_PIN 9

VL53L1X sensor;

void setup() {
    Serial.begin(115200);
    Wire.begin(SDA_PIN, SCL_PIN);
    Wire.setClock(100000);
    delay(100);
    sensor.setBus(&Wire);
    sensor.setTimeout(500);
    if (!sensor.init()) {
        Serial.println("VL53L1X init failed. Check wiring and address 0x29.");
        while (1) delay(1000);
    }
    sensor.setDistanceMode(VL53L1X::Long);
    sensor.setMeasurementTimingBudget(50000);
    sensor.startContinuous(50);
    Serial.println("VL53L1X started");
}

void loop() {
    uint16_t distance = sensor.read();
    if (sensor.timeoutOccurred()) {
        Serial.println("Timeout");
    } else {
        Serial.print("Distance: ");
        Serial.print(distance);
        Serial.println(" mm");
    }
    delay(100);
}

