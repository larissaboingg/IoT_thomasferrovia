#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include <DHT.h>

#define DHTPIN 4
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

WiFiClientSecure client;        //cria cliente wifi
PubSubClient mqtt(client);  //fala que mqtt usa o cliente wifi

const String SSID = "FIESC_IOT_EDU";
const String PASS = "8120gv08";

const String URL         = "c2388c77116243a0a57bd57d29c9b376.s1.eu.hivemq.cloud";
const int PORT           = 8883;


const String broker_user = "Sensor1";
const String broker_pass = "Gabriela123";

// ##CRIAR OS TOPICOS
// -presenca
// -temperatura
// -umidade
// -iluminacao <-inscrever

const char* topicoPresenca_1    = "presenca_1";      //define de onde vou receber as msgs
const char* topicoTemperatura    = "temperatura";
const char* topicoUmidade       = "umidade";
const char* topicoIluminacao    = "iluminacao";

const int ledPin = 19;
const int pinoLDR = 34;
const int pinoUltra_echo = 23;
const int pinoUltra_trig = 22;


void setup() {
  Serial.begin(115200);
  dht.begin();
  pinMode(ledPin, OUTPUT);
  pinMode(pinoUltra_trig, OUTPUT);
  pinMode(pinoUltra_echo, INPUT);


  Serial.println("Conectando ao Wifi");
  WiFi.begin(SSID,PASS);
  while(WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(200);
  }
  Serial.println("\nConectado!");
  client.setInsecure();
  Serial.println("Conectando ao broker...");
  mqtt.setServer(URL.c_str(),PORT);
  while(!mqtt.connected()){
    String ID = "S1-";
    ID += String(random(0xffff),HEX);
    mqtt.connect(ID.c_str(),broker_user.c_str(),broker_pass.c_str());
    delay(200);
    Serial.print(".");
  }

  mqtt.subscribe(topicoIluminacao);   
  mqtt.setCallback(callback);
  
  Serial.println("\nConectado ao broker com sucesso!");
  pinMode(ledPin, OUTPUT);
}

long lerDistancia() {
  digitalWrite(pinoUltra_trig, LOW);
  delayMicroseconds(2);
  digitalWrite(pinoUltra_trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(pinoUltra_trig, LOW);
  
  long duracao = pulseIn(pinoUltra_echo, HIGH);
  long distancia = duracao * 349.24 / 2 / 10000;
  
  return distancia;
}

void loop() {
  
  int leituraLDR = analogRead(pinoLDR);
  float tensao = (leituraLDR * 3.3) / 4095.0;
  
  Serial.print("Leitura LDR: ");
  Serial.print(leituraLDR);
  Serial.print(" - Tensão: ");
  Serial.println(tensao);
  if (leituraLDR < 2800) {
    mqtt.publish(topicoIluminacao,"acender");  
  } else {
    mqtt.publish(topicoIluminacao,"apagar"); 
  }

  
  long distancia = lerDistancia();
  
  Serial.print("Distância: ");
  Serial.print(distancia);
  Serial.println(" cm");
  
  if (distancia < 10) {
    mqtt.publish(topicoPresenca_1,"detectado"); 
  }


  float umidade = dht.readHumidity();
  float temperatura = dht.readTemperature();
  
  if (isnan(umidade) || isnan(temperatura)) {
    Serial.println("Erro na leitura do DHT11");
  }else{
    mqtt.publish(topicoUmidade, String(umidade).c_str()); 
    mqtt.publish(topicoTemperatura, String(temperatura).c_str());
  }
  
  Serial.print("Umidade: ");
  Serial.print(umidade); 
  Serial.print("%  Temperatura: ");
  Serial.print(temperatura); 
  Serial.println("°C");

  mqtt.loop();
  delay(500);
}

void callback(char* topic, byte* payload, unsigned int length){
  String mensagem = "";
  for(int i = 0; i < length; i++){
    mensagem += (char)payload[i];
  }
  Serial.print("Recebido: ");
  Serial.println(mensagem);
  if(mensagem == "acender"){
    digitalWrite(2, HIGH);
  }else if (mensagem == "apagar"){
    digitalWrite(2, LOW);
  }
}








