#include <SoftwareSerial.h>
#include <DHT.h>

// Definir pines y tipo de sensor DHT
#define PIN_DHT 9
#define DHTTYPE DHT11 // Pin  sensor de humedad y temperatura
#define PIN_HUM_SUELO A0
#define PIN_LUZ A2
#define PIN_ENCODER_A 2
#define PIN_ENCODER_B 3

// Variables globales para sensores
float humAmbiente = 0.0; // Humedad ambiental
float tempAmbiente = 0.0; // Temperatura ambiental
float humSuelo = 0.0;
float valorLDR = 0.0;
float viento = 0.0;
volatile unsigned int temp, contador = 0; // Contador aumenta o decrese dependiendo de la rotacion del encoder
DHT dht(PIN_DHT, DHTTYPE);

SoftwareSerial espSerial(3, 4); // RX, TX de la nodeMCU

void setup() {
  Serial.begin(9600); 
  dht.begin();
  pinMode(PIN_LUZ, INPUT); // Configuración para sensor de luz
  pinMode(PIN_ENCODER_A, INPUT_PULLUP); // Configuracion para encoder rotatorio Fase A
  pinMode(PIN_ENCODER_B, INPUT_PULLUP); // Configuracion para encoder rotatorio Fase B
  //Interrupciones encoder rotatorio
  // Un pulso A ascendente del codificador activa ai0()
  attachInterrupt(digitalPinToInterrupt(PIN_ENCODER_A), ai0, RISING); 
   
  // Un pulso B ascendente del codificador activa ai1()
  attachInterrupt(digitalPinToInterrupt(PIN_ENCODER_B), ai1, RISING);

  delay(1000); // Espera a que nodeMCU se inicialice
}

void loop() {
  // Aca tenemos datos harcodeados pero fue ponele q lo lees del sensor
  tempAmbiente = dht.readTemperature();
  humAmbiente = dht.readHumidity();
  humSuelo = analogRead(PIN_HUM_SUELO);
  valorLDR = analogRead(PIN_LUZ);

  float velocidadViento = 15.3;

  // Enviamos los datos a la nodeMCU imprimiendo en el serial
  //Serial.println("Enviar datos"); //IMPORTANTE NO IMPRIMIR OTRAS COSAS EN LA PLACA QUE ENVIA
  Serial.print("[");
  Serial.print(tempAmbiente);
  Serial.print(',');
  Serial.print(humAmbiente);
  Serial.print(',');
  //Serial.printl(viento);
  //Serial.print(',');
  Serial.print(humSuelo);
  Serial.print(',');
  Serial.print(valorLDR);
  Serial.print("]");

  delay(15000); // Hay que esperar para mandar los datos a la proxima pasada del loop
  if( contador != temp ){
    viento = contador;
    temp = contador;
  }

}

void ai0() {
  // ai0 se activa si el pin digital 2 va de LOW a HIGH
  // Usar pin 3 para determinar la direccion
  if(digitalRead(3)==LOW) {
    contador++;
  } else {
    contador--;
  }
}
   
void ai1() {
  // ai0 is activated if DigitalPin nr 3 is going from LOW to HIGH
  // Check with pin 2 to determine the direction
  if(digitalRead(2)==LOW) {
    contador--;
  } else {
    contador++;
  }
}
