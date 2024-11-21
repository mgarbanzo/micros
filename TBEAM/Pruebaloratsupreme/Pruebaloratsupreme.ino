#include "LoRaBoards.h"
#include <RadioLib.h>
#include <TinyGPS++.h>
// busca que reseptor lora se utiliza 
#if     defined(USING_SX1276)
#ifndef CONFIG_RADIO_FREQ
#define CONFIG_RADIO_FREQ           868.0
#endif
#ifndef CONFIG_RADIO_OUTPUT_POWER
#define CONFIG_RADIO_OUTPUT_POWER   17
#endif
#ifndef CONFIG_RADIO_BW
#define CONFIG_RADIO_BW             125.0
#endif
SX1276 radio = new Module(RADIO_CS_PIN, RADIO_DIO0_PIN, RADIO_RST_PIN, RADIO_DIO1_PIN);

#elif   defined(USING_SX1278)
#ifndef CONFIG_RADIO_FREQ
#define CONFIG_RADIO_FREQ           433.0
#endif
#ifndef CONFIG_RADIO_OUTPUT_POWER
#define CONFIG_RADIO_OUTPUT_POWER   17
#endif
#ifndef CONFIG_RADIO_BW
#define CONFIG_RADIO_BW             125.0
#endif
SX1278 radio = new Module(RADIO_CS_PIN, RADIO_DIO0_PIN, RADIO_RST_PIN, RADIO_DIO1_PIN);

#elif   defined(USING_SX1262)
#ifndef CONFIG_RADIO_FREQ
#define CONFIG_RADIO_FREQ           915.0
#endif
#ifndef CONFIG_RADIO_OUTPUT_POWER
#define CONFIG_RADIO_OUTPUT_POWER   22
#endif
#ifndef CONFIG_RADIO_BW
#define CONFIG_RADIO_BW             125.0
#endif

SX1262 radio = new Module(RADIO_CS_PIN, RADIO_DIO1_PIN, RADIO_RST_PIN, RADIO_BUSY_PIN);

#elif   defined(USING_SX1280)
#ifndef CONFIG_RADIO_FREQ
#define CONFIG_RADIO_FREQ           2400.0
#endif
#ifndef CONFIG_RADIO_OUTPUT_POWER
#define CONFIG_RADIO_OUTPUT_POWER   13
#endif
#ifndef CONFIG_RADIO_BW
#define CONFIG_RADIO_BW             203.125
#endif
SX1280 radio = new Module(RADIO_CS_PIN, RADIO_DIO1_PIN, RADIO_RST_PIN, RADIO_BUSY_PIN);

#elif  defined(USING_SX1280PA)
#ifndef CONFIG_RADIO_FREQ
#define CONFIG_RADIO_FREQ           2400.0
#endif
#ifndef CONFIG_RADIO_OUTPUT_POWER
#define CONFIG_RADIO_OUTPUT_POWER   3           // PA Version power range : -18 ~ 3dBm
#endif
#ifndef CONFIG_RADIO_BW
#define CONFIG_RADIO_BW             203.125
#endif
SX1280 radio = new Module(RADIO_CS_PIN, RADIO_DIO1_PIN, RADIO_RST_PIN, RADIO_BUSY_PIN);

#elif   defined(USING_SX1268)
#ifndef CONFIG_RADIO_FREQ
#define CONFIG_RADIO_FREQ           433.0
#endif
#ifndef CONFIG_RADIO_OUTPUT_POWER
#define CONFIG_RADIO_OUTPUT_POWER   22
#endif
#ifndef CONFIG_RADIO_BW
#define CONFIG_RADIO_BW             125.0
#endif
SX1268 radio = new Module(RADIO_CS_PIN, RADIO_DIO1_PIN, RADIO_RST_PIN, RADIO_BUSY_PIN);

#elif   defined(USING_LR1121)

// The maximum power of LR1121 2.4G band can only be set to 13 dBm
// #ifndef CONFIG_RADIO_FREQ
// #define CONFIG_RADIO_FREQ           2450.0
// #endif
// #ifndef CONFIG_RADIO_OUTPUT_POWER
// #define CONFIG_RADIO_OUTPUT_POWER   13
// #endif
// #ifndef CONFIG_RADIO_BW
// #define CONFIG_RADIO_BW             125.0
// #endif

// The maximum power of LR1121 Sub 1G band can only be set to 22 dBm
#ifndef CONFIG_RADIO_FREQ
#define CONFIG_RADIO_FREQ           868.0
#endif
#ifndef CONFIG_RADIO_OUTPUT_POWER
#define CONFIG_RADIO_OUTPUT_POWER   22
#endif
#ifndef CONFIG_RADIO_BW
#define CONFIG_RADIO_BW             125.0
#endif

LR1121 radio = new Module(RADIO_CS_PIN, RADIO_DIO9_PIN, RADIO_RST_PIN, RADIO_BUSY_PIN);
#endif

// this function is called when a complete packet
// is transmitted by the module
// IMPORTANT: this function MUST be 'void' type
//            and MUST NOT have any arguments!
// The TinyGPS++ object para controlar el gps
TinyGPSPlus gps;
#define MAX_LINE_LENGTH 21 // Maximum characters per line on the OLED
//definicion de variables de estado
// guarda el estado de la transmicion
static int transmissionState = RADIOLIB_ERR_NONE;
// indicador de envio de paquete
static volatile bool transmittedFlag = false;
// variable que guarda el paquete a enviar
static String datatrans;
//variables para transmicion en el paquete
String estadogps;
String identificador="A";
String numsat;
String lat;
String lon;
// se define cada cuanto se envia un paquete
int timeinit=millis();
int timesend=1000;
void setFlag(void)
{
    // we sent a packet, set the flag
    transmittedFlag = true;
}

void setup()
{
//apartado de configuracion del lora 
    setupBoards();

    // When the power is turned on, a delay is required.
    delay(1500);
    Serial.begin(9600);
        if (!u8g2)
    {
        Serial.println("No SH1106 display found! Please check the connection.");
        while (1);
    }

#ifdef  RADIO_TCXO_ENABLE
    pinMode(RADIO_TCXO_ENABLE, OUTPUT);
    digitalWrite(RADIO_TCXO_ENABLE, HIGH);
#endif

    // initialize radio with default settings
    int state = radio.begin();

    printResult(state == RADIOLIB_ERR_NONE);
    String lora;

    Serial.print(F("Radio Initializing ... "));
    if (state == RADIOLIB_ERR_NONE) {
        Serial.println(F("success!"));
        lora="success!";
    } else {
        Serial.print(F("failed, code "));
        Serial.println(state);
        lora="Fail code";
        while (true);
    }

    // set the function that will be called
    // se define que funcion utilizar para verificar envio de dato
    radio.setPacketSentAction(setFlag);
// Se definen parametros para el envio de paketes 
    radio.setFrequency(915.0);            // Frecuencia 
    radio.setCodingRate(5);             // Coding Rate  (4/5)
    radio.setSyncWord(0xAB);            // Sync Word 
    radio.setCRC(false);                // CRC deshabilitado
    // start transmitting the first packet
    //Serial.print(F("Radio Sending first packet ... "));

    // you can transmit C-string or Arduino string up to
    // 256 characters long
    //transmissionState = radio.startTransmit(String(counter).c_str());

    // you can also transmit byte array up to 256 bytes long
    /*
      byte byteArr[] = {0x01, 0x23, 0x45, 0x67,
                        0x89, 0xAB, 0xCD, 0xEF};
      state = radio.startTransmit(byteArr, 8);
    */
    delay(1000);
// apartado de preparacion pantalla

// imprime en pantalla el estado del reseptor lora y un mesage de busqueda de gps 
    u8g2->clearBuffer();
    u8g2->setFont(u8g2_font_5x8_tr); // Set a compact font for better text fitting
    u8g2->drawStr(0, 10, String("Lora init "+lora).c_str());
     u8g2->sendBuffer();
    u8g2->drawStr(5, 20, "Initializing GPS...");
    u8g2->sendBuffer();

    Serial.println(F("GPS OLED Example"));
    Serial.println(F("Displaying GPS data on SH1106 OLED"));
}

void loop()
{
    static const double LONDON_LAT = 51.508131, LONDON_LON = -0.128002;

    // Clear the display buffer
    u8g2->clearBuffer();
    // revisa si hay coneccion GPS
       if (gps.location.isValid())
    {
      estadogps=String(1);
        // Latitude and Longitude
        char latStr[MAX_LINE_LENGTH];
        char lonStr[MAX_LINE_LENGTH];
        snprintf(latStr, MAX_LINE_LENGTH, "Lat: %.6f", gps.location.lat());
        snprintf(lonStr, MAX_LINE_LENGTH, "Lon: %.6f", gps.location.lng());
        lat=String(gps.location.lat());
        lon=String(gps.location.lng());


        u8g2->drawStr(0, 10, latStr);
        u8g2->drawStr(0, 20, lonStr);

        // Satellites and HDOP
        char satStr[MAX_LINE_LENGTH];
        char hdopStr[MAX_LINE_LENGTH];
        snprintf(satStr, MAX_LINE_LENGTH, "Sats: %d", gps.satellites.value());
        snprintf(hdopStr, MAX_LINE_LENGTH, "HDOP: %.1f", gps.hdop.hdop());
        numsat=String(gps.satellites.value());

        u8g2->drawStr(0, 30, satStr);
        u8g2->drawStr(40, 30, hdopStr);

        // Altitude and Speed
        char altStr[MAX_LINE_LENGTH];
        char speedStr[MAX_LINE_LENGTH];
        snprintf(altStr, MAX_LINE_LENGTH, "Alt: %.2f m", gps.altitude.meters());
        snprintf(speedStr, MAX_LINE_LENGTH, "Speed: %.2f km/h", gps.speed.kmph());

        u8g2->drawStr(0, 40, altStr);
        u8g2->drawStr(0, 50, speedStr);
        // se prepara el mensage que se va a mandar en formato identificador,estado del gps,numero de satelites, latitud, longitud
        datatrans= identificador + ","+estadogps+","+numsat+","+lat+","+lon;
        // envia datos al buffer del display 
        u8g2->sendBuffer();
    }
    else
    {
      estadogps=String(0);
        // Display a message if no valid GPS data is available
        datatrans= identificador + ","+estadogps;
        u8g2->drawStr(0, 10, "Waiting for GPS2...");
         u8g2->sendBuffer();
    }
    smartDelay(1000);
  // envia datos por lora 
  if (millis()-timeinit>timesend){
    timeinit=millis();
    if (transmittedFlag) {


        // reinicia la variable identificadora de envio de senal, si esta es true es porque ya se envio un pakete
        transmittedFlag = false;

        flashLed();


        if (transmissionState == RADIOLIB_ERR_NONE) {
            // mensages de que se envio el dato
            Serial.println(F("transmission finished!"));
            u8g2->drawStr(5, 60, "TF");
            u8g2->sendBuffer();
            // NOTE: when using interrupt-driven transmit method,
            //       it is not possible to automatically measure
            //       transmission data rate using getDataRate()
        } else {
            Serial.print(F("failed, code "));
            Serial.println(transmissionState);
        }


        //drawMain();
        // wait a second before transmitting again
        delay(1000);

        // send another one
        Serial.print(F("Radio Sending another packet ... "));

        // you can transmit C-string or Arduino string up to
        // 256 characters long
        transmissionState = radio.startTransmit(datatrans);
        // you can also transmit byte array up to 256 bytes long
        /*
          byte byteArr[] = {0x01, 0x23, 0x45, 0x67,
                            0x89, 0xAB, 0xCD, 0xEF};
          int state = radio.startTransmit(byteArr, 8);
        */

    }
  }
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



