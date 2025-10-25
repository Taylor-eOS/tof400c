#include <Arduino.h>
#include <U8g2lib.h>
#include <VL53L1X.h>
#include <Wire.h>

#define SENSOR_SDA_PIN 8
#define SENSOR_SCL_PIN 9
#define DISPLAY_SDA_PIN 5
#define DISPLAY_SCL_PIN 6
#define I2C_ADDR 0x3C
#define USABLE_WIDTH 72
#define USABLE_HEIGHT 40

U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, DISPLAY_SCL_PIN, DISPLAY_SDA_PIN, U8X8_PIN_NONE);

const uint8_t offsetX = 28;
const uint8_t offsetY = 24;
const bool testPixels = false;
const bool invertDisplay = false;
const uint8_t brightness = 255;
VL53L1X sensor;
uint16_t distance = 0;
bool isTimeout = false;
char displayText[32];

void drawDisplay() {
    u8g2.clearBuffer();
    if (testPixels) {
        u8g2.drawPixel(offsetX, offsetY);
        u8g2.drawPixel(offsetX + USABLE_WIDTH - 1, offsetY);
        u8g2.drawPixel(offsetX, offsetY + USABLE_HEIGHT - 1);
        u8g2.drawPixel(offsetX + USABLE_WIDTH - 1, offsetY + USABLE_HEIGHT - 1);
    }
    u8g2.setFont(u8g2_font_logisoso32_tn);
    if (isTimeout) {
        snprintf(displayText, sizeof(displayText), "----");
    } else {
        snprintf(displayText, sizeof(displayText), "%u", distance);
    }
    uint8_t textWidth = u8g2.getStrWidth(displayText);
    uint8_t fontHeight = u8g2.getMaxCharHeight();
    uint8_t x = offsetX + (USABLE_WIDTH - textWidth) / 2;
    uint8_t y = offsetY + fontHeight;
    u8g2.drawStr(x, y, displayText);
    u8g2.sendBuffer();
}

void setup() {
    Serial.begin(115200);
    Wire.begin(SENSOR_SDA_PIN, SENSOR_SCL_PIN);
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
    u8g2.setI2CAddress(I2C_ADDR * 2);
    u8g2.begin();
    u8g2.setBusClock(400000UL);
    u8g2.setContrast(brightness);
    if (invertDisplay) u8g2.setDisplayRotation(U8G2_R2);
    delay(50);
    drawDisplay();
}

void loop() {
    distance = sensor.read();
    if (sensor.timeoutOccurred()) {
        isTimeout = true;
        Serial.println("Timeout");
    } else {
        isTimeout = false;
        Serial.print("Distance: ");
        Serial.print(distance);
        Serial.println(" mm");
    }
    drawDisplay();
    delay(100);
}

