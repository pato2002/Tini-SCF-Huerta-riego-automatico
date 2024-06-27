#include <DHT.h>

// Definir pines y tipo de sensor DHT
#define PIN_DHT 9
#define DHTTYPE DHT11 // Pin  sensor de humedad y temperatura
#define PIN_HUM_SUELO A0
#define PIN_LUZ A2

// Variables globales para sensores
float humAmbiente = 0.0; // Humedad ambiental
float tempAmbiente = 0.0; // Temperatura ambiental
float humSuelo = 0.0;
float valorLDR = 0.0;
DHT dht(PIN_DHT, DHTTYPE);

void setup()
{
    Serial.begin(9600);
    dht.begin();
    pinMode(PIN_LUZ, INPUT); // Configuración para sensor de luz
    delay(1000); // Espera a que nodeMCU se inicialice
}

void loop()
{
    tempAmbiente = dht.readTemperature();
    humAmbiente = dht.readHumidity();
    humSuelo = analogRead(PIN_HUM_SUELO);
    valorLDR = analogRead(PIN_LUZ);

    // Enviamos los datos a la nodeMCU imprimiendo en el serial
    // Serial.println("Enviar datos"); //IMPORTANTE NO IMPRIMIR OTRAS COSAS EN LA PLACA QUE ENVIA
    Serial.print("[");
    Serial.print(tempAmbiente);
    Serial.print(',');
    Serial.print(humAmbiente);
    Serial.print(',');
    Serial.print(humSuelo);
    Serial.print(',');
    Serial.print(valorLDR);
    Serial.print("]");

    delay(15000); // Hay que esperar para mandar los datos a la proxima pasada del loop
}