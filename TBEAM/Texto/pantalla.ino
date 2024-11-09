#include <Arduino.h>
#include "LoRaBoards.h"

void setup(void)
{
    setupBoards();
    if (!u8g2) {
        Serial.println("No SH1106 display found! Please check the connection.");
        while (1);
    }
    u8g2->enableUTF8Print();  // Enable UTF8 support for the Arduino print() function
}

void loop(void)
{
    // Use a font that supports Latin-1 Supplement characters (including accented letters)
    u8g2->setFont(u8g2_font_unifont_t_extended);  // Supports accented characters
    u8g2->setFontDirection(0);
    u8g2->clearBuffer();

    // Display Spanish text with accented characters
    u8g2->setCursor(0, 15);
    u8g2->print("Universidad de");
    u8g2->setCursor(0, 30);
    u8g2->print("Costa Rica");
    u8g2->setCursor(0, 45);
    u8g2->print("Escuela de");
    u8g2->setCursor(0, 60);
    u8g2->print("Física - LOSiC");

    u8g2->sendBuffer();
    delay(1000);
}
