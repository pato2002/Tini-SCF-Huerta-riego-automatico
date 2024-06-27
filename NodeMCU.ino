#include "ThingSpeak.h"
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include "private.ino";
#define RED D0 // Pin del LED rojo ni idea donde esta
#define YELLOW D1 // Pin del LED amarillo
#define GREEN D2 // Pin del LED verde

unsigned long myChannel = credentials.ts_channel;
const char* writeAPIKey =  credentials.write_api_key;
const char* talkBackAPIKey = credentials.tb_api_key;
const char* talkBackID = credentials.tb_id;
const int fieldTempAmbiente = 1;
const int fieldHumAmbiente = 2;
const int fieldHumSuelo = 3;
const int fieldValorLDR = 4;
const float tempAmbiente = 0.0;
const float humAmbiente = 0.0;
const float humSuelo = 0.0;
const float valorLDR = 0.0;
const char* server = "api.thingspeak.com";

// WiFi credentials
const char ssid[] = credentials.ssid;
const char pass[] = credentials.password;
WiFiClient client;

void connectWifi()
{
    Serial.println("Attempting to connect to Wifi...");
    WiFi.disconnect(true); // Desconecta y borra configuraciones previas
    delay(1000); // Espera un momento para asegurar la desconexión
    WiFi.begin(ssid, pass);

    int retryCount = 0;
    while (WiFi.status() != WL_CONNECTED && retryCount < 10) {
        delay(1000);
        Serial.print(".");
        retryCount++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nConnected to WiFi");
        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("\nFailed to connect to WiFi");
    }
}

void processPacket(String packet)
{
    Serial.println("Comenzando envio");
    float numbers[4];
    int index = 0;
    int start = 0;
    int end = packet.indexOf(',');

    while (end > 0) {
        numbers[index++] = packet.substring(start, end).toFloat();
        start = end + 1;
        end = packet.indexOf(',', start);
    }
    numbers[index] = packet.substring(start).toFloat();

    // Procesar los números y enviarlos a ThingSpeak
    Serial.print("Temperatura ambiente: ");
    Serial.println(numbers[0]);
    Serial.print("Humedad ambiente: ");
    Serial.println(numbers[1]);
    Serial.print("Humedad del suelo: ");
    Serial.println(numbers[2]);
    Serial.print("Valor del sensor de luz: ");
    Serial.println(numbers[3]);

    ThingSpeak.setField(fieldTempAmbiente, numbers[0]);
    ThingSpeak.setField(fieldHumAmbiente, numbers[1]);
    ThingSpeak.setField(fieldHumSuelo, numbers[2]);
    ThingSpeak.setField(fieldValorLDR, numbers[3]);
    enviar_a_canal();
    delay(20000);
}

void enviar_a_canal()
{
    int httpCode = ThingSpeak.writeFields(myChannel, writeAPIKey);
    if (httpCode == 200) {
        Serial.println("Channel write successful.");
        Serial.println("-------------------------");
    } else {
        Serial.println("HTTP error code " + String(httpCode));
    }
}

void encenderLed(int pin)
{
    digitalWrite(pin, HIGH); // Encender el LED
    delay(1000); // Esperar 1 segundo
    digitalWrite(pin, LOW); // Apagar el LED
}

String getTalkBackCommand()
{
    HTTPClient http;
    String url = String("http://api.thingspeak.com/talkbacks/") + talkBackID + "/commands/execute?api_key=" + talkBackAPIKey;

    http.begin(client, url);
    int httpCode = http.GET();
    String payload = "";

    if (httpCode > 0) {
        if (httpCode == HTTP_CODE_OK) {
            payload = http.getString();
            Serial.print("Comando recibido: ");
            Serial.println(payload);
        } else {
            Serial.print("Error al obtener comando, código HTTP: ");
            Serial.println(httpCode);
        }
    } else {
        Serial.print("Error al realizar la solicitud HTTP: ");
        Serial.println(http.errorToString(httpCode).c_str());
    }

    http.end();
    return payload;
}

void setup()
{
    Serial.begin(9600);
    pinMode(RED, OUTPUT);
    pinMode(YELLOW, OUTPUT);
    pinMode(GREEN, OUTPUT);
    WiFi.mode(WIFI_STA);
    connectWifi(); // Conectar al WiFi al iniciar
    ThingSpeak.begin(client);
    delay(5000);
}

void loop()
{
    if (WiFi.status() != WL_CONNECTED) {
        connectWifi();
    }

    if (Serial.available()) {
        String packet = Serial.readStringUntil(']');
        if (packet.startsWith("[")) {
            packet = packet.substring(1); // Eliminar el carácter de inicio '['
            processPacket(packet);
        }
    }

    String command = getTalkBackCommand();

    if (command == "ROJO") {
        digitalWrite(RED, HIGH); // Encender el LED
        digitalWrite(YELLOW, LOW); // Apagar el LED
        digitalWrite(GREEN, LOW); // Apagar el LED
    } else if (command == "AMARILLO") {
        digitalWrite(YELLOW, HIGH); // Encender el LED
        digitalWrite(RED, LOW); // Apagar el LED
        digitalWrite(GREEN, LOW); // Apagar el LED
    } else if (command == "VERDE") {
        digitalWrite(GREEN, HIGH); // Encender el LED
        digitalWrite(RED, LOW); // Apagar el LED
        digitalWrite(YELLOW, LOW); // Apagar el LED
    }
}
