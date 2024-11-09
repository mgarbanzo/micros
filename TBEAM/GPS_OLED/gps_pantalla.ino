#include <TinyGPS++.h>
#include "LoRaBoards.h"

// The TinyGPS++ object
TinyGPSPlus gps;

// Constants for display updates
#define MAX_LINE_LENGTH 21 // Maximum characters per line on the OLED

void setup()
{
    setupBoards();

    if (!u8g2)
    {
        Serial.println("No SH1106 display found! Please check the connection.");
        while (1);
    }

    u8g2->clearBuffer();
    u8g2->setFont(u8g2_font_5x8_tr); // Set a compact font for better text fitting
    u8g2->drawStr(0, 10, "Initializing GPS...");
    u8g2->sendBuffer();

    Serial.println(F("GPS OLED Example"));
    Serial.println(F("Displaying GPS data on SH1106 OLED"));
}

void loop()
{
    static const double LONDON_LAT = 51.508131, LONDON_LON = -0.128002;

    // Clear the display buffer
    u8g2->clearBuffer();

    // Check if GPS data is valid
    if (gps.location.isValid())
    {
        // Latitude and Longitude
        char latStr[MAX_LINE_LENGTH];
        char lonStr[MAX_LINE_LENGTH];
        snprintf(latStr, MAX_LINE_LENGTH, "Lat: %.6f", gps.location.lat());
        snprintf(lonStr, MAX_LINE_LENGTH, "Lon: %.6f", gps.location.lng());

        u8g2->drawStr(0, 10, latStr);
        u8g2->drawStr(0, 20, lonStr);

        // Satellites and HDOP
        char satStr[MAX_LINE_LENGTH];
        char hdopStr[MAX_LINE_LENGTH];
        snprintf(satStr, MAX_LINE_LENGTH, "Sats: %d", gps.satellites.value());
        snprintf(hdopStr, MAX_LINE_LENGTH, "HDOP: %.1f", gps.hdop.hdop());

        u8g2->drawStr(0, 30, satStr);
        u8g2->drawStr(0, 40, hdopStr);

        // Altitude and Speed
        char altStr[MAX_LINE_LENGTH];
        char speedStr[MAX_LINE_LENGTH];
        snprintf(altStr, MAX_LINE_LENGTH, "Alt: %.2f m", gps.altitude.meters());
        snprintf(speedStr, MAX_LINE_LENGTH, "Speed: %.2f km/h", gps.speed.kmph());

        u8g2->drawStr(0, 50, altStr);
        u8g2->drawStr(0, 60, speedStr);
    }
    else
    {
        // Display a message if no valid GPS data is available
        u8g2->drawStr(0, 10, "Waiting for GPS...");
    }

    // Send buffer to the display
    u8g2->sendBuffer();

    // Process GPS data
    smartDelay(1000);
}

// This custom version of delay() ensures that the GPS object is being "fed".
static void smartDelay(unsigned long ms)
{
    unsigned long start = millis();
    do
    {
        while (SerialGPS.available())
            gps.encode(SerialGPS.read());
    } while (millis() - start < ms);
}
