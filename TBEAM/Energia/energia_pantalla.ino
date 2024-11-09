#include "LoRaBoards.h"

#if !defined(T_BEAM_S3_SUPREME) && !defined(T_BEAM)
#error "Este código solo es compatible con placas que usen los chips AXP192 o AXP2101."
#endif

uint32_t inicioMilisegundos;

void setup()
{
    // Inicializar PMU y OLED
    setupBoards();

    if (!u8g2)
    {
        Serial.println("¡No se detectó la pantalla SH1106! Verifique la conexión.");
        while (1);
    }

    // Mostrar mensaje inicial en la pantalla
    u8g2->clearBuffer();
    u8g2->setFont(u8g2_font_5x8_tr); // Usar fuente compacta
    u8g2->drawStr(0, 10, "Inicializando PMU...");
    u8g2->sendBuffer();

    inicioMilisegundos = millis();
}

void loop()
{
    // Actualizar los datos del PMU cada segundo
    if (millis() - inicioMilisegundos > 1000)
    {
        inicioMilisegundos = millis();

        // Limpiar el buffer de la pantalla
        u8g2->clearBuffer();

        // Mostrar estado de carga y descarga
        u8g2->setCursor(0, 10);
        u8g2->print("Cargando: ");
        u8g2->print(PMU->isCharging() ? "SI" : "NO");

        u8g2->setCursor(0, 20);
        u8g2->print("Descarga: ");
        u8g2->print(PMU->isDischarge() ? "SI" : "NO");

        // Mostrar estado del VBUS
        u8g2->setCursor(0, 30);
        u8g2->print("VBUS: ");
        u8g2->print(PMU->isVbusIn() ? "SI" : "NO");

        // Mostrar voltaje de la batería en voltios
        char voltajeBateria[16];
        snprintf(voltajeBateria, sizeof(voltajeBateria), "Bateria: %.2f V", PMU->getBattVoltage() / 1000.0);
        u8g2->setCursor(0, 40);
        u8g2->print(voltajeBateria);

        // Mostrar voltaje del sistema en voltios
        char voltajeSistema[16];
        snprintf(voltajeSistema, sizeof(voltajeSistema), "Sistema: %.2f V", PMU->getSystemVoltage() / 1000.0);
        u8g2->setCursor(0, 50);
        u8g2->print(voltajeSistema);

        // Mostrar porcentaje de batería
        if (PMU->isBatteryConnect())
        {
            char porcentajeBateria[16];
            snprintf(porcentajeBateria, sizeof(porcentajeBateria), "Bateria: %d%%", PMU->getBatteryPercent());
            u8g2->setCursor(0, 60);
            u8g2->print(porcentajeBateria);
        }

        // Enviar el contenido al display
        u8g2->sendBuffer();
    }

    // Manejar interrupciones del PMU
    if (pmuInterrupt)
    {
        pmuInterrupt = false;

        // Obtener el registro de estado de las interrupciones del PMU
        uint32_t estado = PMU->getIrqStatus();

        // Manejar eventos específicos del PMU
        if (PMU->isVbusInsertIrq())
        {
            Serial.println("VBUS Conectado");
        }
        if (PMU->isVbusRemoveIrq())
        {
            Serial.println("VBUS Desconectado");
        }
        if (PMU->isBatInsertIrq())
        {
            Serial.println("Batería Conectada");
        }
        if (PMU->isBatRemoveIrq())
        {
            Serial.println("Batería Desconectada");
        }
        if (PMU->isPekeyShortPressIrq())
        {
            Serial.println("Presión Corta del Botón de Encendido");
        }
        if (PMU->isPekeyLongPressIrq())
        {
            Serial.println("Presión Larga del Botón de Encendido");
        }
        if (PMU->isBatChagerDoneIrq())
        {
            Serial.println("Carga Completa");
        }
        if (PMU->isBatChagerStartIrq())
        {
            Serial.println("Inicio de Carga");
        }

        // Limpiar el registro de interrupciones del PMU
        PMU->clearIrqStatus();
    }

    delay(10);
}
