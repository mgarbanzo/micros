#include "LoRaBoards.h"
#include <RadioLib.h>
#include <TinyGPS++.h>
#if   defined(USING_SX1262)
#ifndef CONFIG_RADIO_FREQ
#define CONFIG_RADIO_FREQ           915.0
#endif
#ifndef CONFIG_RADIO_OUTPUT_POWER
#define CONFIG_RADIO_OUTPUT_POWER   22
#endif
#ifndef CONFIG_RADIO_BW
#define CONFIG_RADIO_BW             125.0
#endif
#endif
SX1262 radio = new Module(RADIO_CS_PIN, RADIO_DIO1_PIN, RADIO_RST_PIN, RADIO_BUSY_PIN);

// this function is called when a complete packet
// is transmitted by the module
// IMPORTANT: this function MUST be 'void' type
//            and MUST NOT have any arguments!
// The TinyGPS++ object para controlar el gps
TinyGPSPlus gps;
#define MAX_LINE_LENGTH 21 // Maximum characters per line on the OLED
//definicion de variables de estado
// save transmission state between loops
static int transmissionState = RADIOLIB_ERR_NONE;
// flag to indicate that a packet was sent
static volatile bool transmittedFlag = false;
static uint32_t counter = 0;
static String datatrans;
//variables para transmicion
String estadogps;
// se transmite el identificador de equipo
String identificador="D";
// se guarda el numero de satelites
String numsat;
// se guarda la latitud
String lat;
// se guarda la longitud
String lon;
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

    // un atraso a la hora de iniciar el equipo
    delay(1500);
    Serial.begin(9600);
    // variable de la pantalla
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
    // when packet transmission is finished
    radio.setPacketSentAction(setFlag);

    /*
    *   Sets carrier frequency.
    *   SX1278/SX1276 : Allowed values range from 137.0 MHz to 525.0 MHz.
    *   SX1268/SX1262 : Allowed values are in range from 150.0 to 960.0 MHz.
    *   SX1280        : Allowed values are in range from 2400.0 to 2500.0 MHz.
    *   LR1121        : Allowed values are in range from 150.0 to 960.0 MHz, 1900 - 2200 MHz and 2400 - 2500 MHz. Will also perform calibrations.
    * * * */

    if (radio.setFrequency(CONFIG_RADIO_FREQ) == RADIOLIB_ERR_INVALID_FREQUENCY) {
        Serial.println(F("Selected frequency is invalid for this module!"));
        while (true);
    }

    /*
    *   Sets LoRa link bandwidth.
    *   SX1278/SX1276 : Allowed values are 10.4, 15.6, 20.8, 31.25, 41.7, 62.5, 125, 250 and 500 kHz. Only available in %LoRa mode.
    *   SX1268/SX1262 : Allowed values are 7.8, 10.4, 15.6, 20.8, 31.25, 41.7, 62.5, 125.0, 250.0 and 500.0 kHz.
    *   SX1280        : Allowed values are 203.125, 406.25, 812.5 and 1625.0 kHz.
    *   LR1121        : Allowed values are 62.5, 125.0, 250.0 and 500.0 kHz.
    * * * */
    if (radio.setBandwidth(CONFIG_RADIO_BW) == RADIOLIB_ERR_INVALID_BANDWIDTH) {
        Serial.println(F("Selected bandwidth is invalid for this module!"));
        while (true);
    }


    /*
    * Sets LoRa link spreading factor.
    * SX1278/SX1276 :  Allowed values range from 6 to 12. Only available in LoRa mode.
    * SX1262        :  Allowed values range from 5 to 12.
    * SX1280        :  Allowed values range from 5 to 12.
    * LR1121        :  Allowed values range from 5 to 12.
    * * * */
    if (radio.setSpreadingFactor(7) == RADIOLIB_ERR_INVALID_SPREADING_FACTOR) {
        Serial.println(F("Selected spreading factor is invalid for this module!"));
        while (true);
    }

    /*
    * Sets LoRa coding rate denominator.
    * SX1278/SX1276/SX1268/SX1262 : Allowed values range from 5 to 8. Only available in LoRa mode.
    * SX1280        :  Allowed values range from 5 to 8.
    * LR1121        :  Allowed values range from 5 to 8.
    * * * */
    if (radio.setCodingRate(7) == RADIOLIB_ERR_INVALID_CODING_RATE) {
        Serial.println(F("Selected coding rate is invalid for this module!"));
        while (true);
    }

    /*
    * Sets LoRa sync word.
    * SX1278/SX1276/SX1268/SX1262/SX1280 : Sets LoRa sync word. Only available in LoRa mode.
    * * */
    if (radio.setSyncWord(0x12) != RADIOLIB_ERR_NONE) {
        Serial.println(F("Unable to set sync word!"));
        while (true);
    }

    /*
    * Sets transmission output power.
    * SX1278/SX1276 :  Allowed values range from -3 to 15 dBm (RFO pin) or +2 to +17 dBm (PA_BOOST pin). High power +20 dBm operation is also supported, on the PA_BOOST pin. Defaults to PA_BOOST.
    * SX1262        :  Allowed values are in range from -9 to 22 dBm. This method is virtual to allow override from the SX1261 class.
    * SX1268        :  Allowed values are in range from -9 to 22 dBm.
    * SX1280        :  Allowed values are in range from -18 to 13 dBm. PA Version range : -18 ~ 3dBm
    * LR1121        :  Allowed values are in range from -17 to 22 dBm (high-power PA) or -18 to 13 dBm (High-frequency PA)
    * * * */
    if (radio.setOutputPower(CONFIG_RADIO_OUTPUT_POWER) == RADIOLIB_ERR_INVALID_OUTPUT_POWER) {
        Serial.println(F("Selected output power is invalid for this module!"));
        while (true);
    }

    /*
    * Sets preamble length for LoRa or FSK modem.
    * SX1278/SX1276 : Allowed values range from 6 to 65535 in %LoRa mode or 0 to 65535 in FSK mode.
    * SX1262/SX1268 : Allowed values range from 1 to 65535.
    * SX1280        : Allowed values range from 1 to 65535. preamble length is multiple of 4
    * LR1121        : Allowed values range from 1 to 65535.
    * * */
    if (radio.setPreambleLength(16) == RADIOLIB_ERR_INVALID_PREAMBLE_LENGTH) {
        Serial.println(F("Selected preamble length is invalid for this module!"));
        while (true);
    }

    // Enables or disables CRC check of received packets.
    if (radio.setCRC(true) == RADIOLIB_ERR_INVALID_CRC_CONFIGURATION) {
        Serial.println(F("Selected CRC is invalid for this module!"));
        while (true);
    }
#ifdef USING_DIO2_AS_RF_SWITCH
#ifdef USING_SX1262
    // Some SX126x modules use DIO2 as RF switch. To enable
    // this feature, the following method can be used.
    // NOTE: As long as DIO2 is configured to control RF switch,
    //       it can't be used as interrupt pin!
    if (radio.setDio2AsRfSwitch() != RADIOLIB_ERR_NONE) {
        Serial.println(F("Failed to set DIO2 as RF switch!"));
        while (true);
    }
#endif //USING_SX1262
#endif //USING_DIO2_AS_RF_SWITCH

#ifdef RADIO_RX_PIN
    // SX1280 PA Version
    radio.setRfSwitchPins(RADIO_RX_PIN, RADIO_TX_PIN);
#endif

#ifdef RADIO_SWITCH_PIN
    // T-MOTION
    const uint32_t pins[] = {
        RADIO_SWITCH_PIN, RADIO_SWITCH_PIN, RADIOLIB_NC,
    };
    static const Module::RfSwitchMode_t table[] = {
        {Module::MODE_IDLE,  {0,  0} },
        {Module::MODE_RX,    {1, 0} },
        {Module::MODE_TX,    {0, 1} },
        END_OF_MODE_TABLE,
    };
    radio.setRfSwitchTable(pins, table);
#endif




    // start transmitting the first packet
    Serial.print(F("Radio Sending first packet ... "));

    // you can transmit C-string or Arduino string up to
    // 256 characters long
    transmissionState = radio.startTransmit(String(counter).c_str());

    // you can also transmit byte array up to 256 bytes long
    /*
      byte byteArr[] = {0x01, 0x23, 0x45, 0x67,
                        0x89, 0xAB, 0xCD, 0xEF};
      state = radio.startTransmit(byteArr, 8);
    */
    delay(1000);
// apartado de preparacion pantalla

// se imprime en la pantalla los parametros con los que se configuro el lora
    u8g2->clearBuffer();
    u8g2->setFont(u8g2_font_5x8_tr); // Set a compact font for better text fitting
    u8g2->drawStr(0, 10, String("Lora init "+lora).c_str());
    u8g2->drawStr(0, 20, String("BW "+String(CONFIG_RADIO_BW)).c_str());
    u8g2->drawStr(0, 30, String("FREQ "+String(CONFIG_RADIO_FREQ)).c_str());
    u8g2->drawStr(0, 40, String("SpreadingFactor "+String(7)).c_str());
    u8g2->drawStr(0, 50, String("SyncWord "+String("0x12")).c_str());
    //u8g2->drawStr(5, 20, "Initializing GPS...");
    u8g2->sendBuffer();
delay(20000);
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
        // envia datos de latitud solamente al reseptor.
        char latonly[MAX_LINE_LENGTH];
        char lononly[MAX_LINE_LENGTH];
        snprintf(latonly, MAX_LINE_LENGTH, "%.6f", gps.location.lat());
        snprintf(lononly, MAX_LINE_LENGTH, "%.6f", gps.location.lng());
        lat=String(latonly);
        lon=String(lononly);


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
        // se codifgica el mensaje con la informacion 0,0, que los ignora el reseptor oled, luego se manda idendentificador, estado del gps, numero de satelites, latitud y longitud
        datatrans= String(0)+","+String(0)+","+identificador + ","+estadogps+","+numsat+","+lat+","+lon;
        u8g2->sendBuffer();
    }
    else
    {
      estadogps=String(0);
        // se prepara el mensage de no conexion 0,0, que es lo que se ignora luego se manda el identificador y el estado de no conectado que es 0
        datatrans= String(0)+","+String(0)+","+identificador + ","+estadogps;
        u8g2->drawStr(0, 10, "Waiting for GPS2...");
         u8g2->sendBuffer();
    }
    smartDelay(1000);
// se encia mensages cada timesend
  if (millis()-timeinit>timesend){
    timeinit=millis();
    if (transmittedFlag) {


        // identificador de que se esta mandando un mensage
        transmittedFlag = false;
// enciende la led
        flashLed();


        if (transmissionState == RADIOLIB_ERR_NONE) {
            // packet was successfully sent
            Serial.println(F("transmission finished!"));
            // se desplega en la pantalla TF de que termino la transmicion
            u8g2->drawStr(5, 60, "TF");
            u8g2->sendBuffer();
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



