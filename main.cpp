#include "I2C.h"
#include "ThisThread.h"
#include "mbed.h"
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h" 
#include <cstring>

#define tiempo_muestreo   1s
#define TMP102_ADDRESS 0x90

// Pines y puertos 
BufferedSerial serial(USBTX, USBRX);
I2C i2c (D14,D15);
Adafruit_SSD1306_I2c oled (i2c, D0);
AnalogIn ain(A0);

// Variables globales 
float Vin=0.0;
int ent=0, dec=0;
char men[40];
char comando[3]= {0x01, 0x60, 0xA0};
char data[2];
const char *mensaje_inicio = "Arranque del programa\n\r";

// Prototipos de funciones
void inicializarOLED();
void mostrarTextoOLED(const char* mensaje);
void leerYMostrarVoltaje();
void leerYMostrarTemperatura();
void enviarSerial(const char* mensaje);

int main() { 
    // Inicialización
    inicializarOLED();
    i2c.write(TMP102_ADDRESS, comando, 3); // Arranque del sensor de temperatura
    enviarSerial(mensaje_inicio); // Saludo por el puerto serial

    while (true) {
        leerYMostrarVoltaje();
        leerYMostrarTemperatura();
        ThisThread::sleep_for(tiempo_muestreo);
    }
}

// Función para inicializar la pantalla OLED
void inicializarOLED() {
    oled.begin();
    oled.setTextSize(1);
    oled.setTextColor(1);
    oled.display();
    ThisThread::sleep_for(3000ms);
    oled.clearDisplay();
    oled.display();
    oled.printf("Test\r\n");
    oled.display();
}

// Función para mostrar texto en la pantalla OLED
void mostrarTextoOLED(const char* mensaje) {
    oled.clearDisplay();
    oled.display();
    oled.setTextCursor(0, 2);
    oled.printf(mensaje);
    oled.display();
}

// Función para leer el voltaje del potenciómetro y mostrarlo
void leerYMostrarVoltaje() {
    Vin = ain * 3.3; // Lectura del sensor analógico
    ent = int(Vin); 
    dec = int((Vin - ent) * 10000); 
    sprintf(men, "El voltaje es:\n\r %01u.%04u volts\n\r", ent, dec);
    mostrarTextoOLED(men);
    enviarSerial(men);
}

// Función para leer y mostrar la temperatura desde el sensor TMP102
void leerYMostrarTemperatura() {
    comando[0] = 0; // Registro de temperatura
    i2c.write(TMP102_ADDRESS, comando, 1); // Enviar el comando para leer
    i2c.read(TMP102_ADDRESS, data, 2); // Leer 2 bytes de temperatura
    int16_t temp = (data[0] << 4) | (data[1] >> 4);
    float Temperatura = temp * 0.0625; 
    ent = int(Temperatura); 
    dec = int((Temperatura - ent) * 10000);
    sprintf(men, "La Temperatura es:\n\r %01u.%04u Celsius\n\r", ent, dec);
    mostrarTextoOLED(men);
    enviarSerial(men);
}

// Función para enviar mensajes por el puerto serial
void enviarSerial(const char* mensaje) {
    serial.write(mensaje, strlen(mensaje));
}
