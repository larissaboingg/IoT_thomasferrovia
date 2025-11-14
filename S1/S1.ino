#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <DHT.h>

#define DHTPIN 4
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

WiFiClientSecure client;        //cria cliente wifi
PubSubClient mqtt(client);  //fala que mqtt usa o cliente wifi

const String SSID = "FIESC_IOT_EDU";
const String PASS = "8120gv08";

const int PORT           =1883;
const String URL         ="test.mosquitto.org";


#DEFINIR USER E SENHA NO HIVEMQ
const String broker_user ="";
const String broker_pass = "";

##CRIAR OS TOPICOS
-presenca
-temperatura
-umidade
-iluminacao <-inscrever

const char* presenca_1    = "presenca_1";      //define de onde vou receber as msgs
const char* tempertura    = "temperatura";
const char* umidade       = "umidade";
const char* iluminacao    = "iluminacao";

const int ledPin = 2;
const int pinoLDR = 34;
const int pinoDHT = 4;
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
  mqtt.subscribe(       ); <-botar o topico iluminacao
  mqtt.setCallback(callback);
  
  Serial.println("\nConectado ao broker com sucesso!");
  pinMode(ledAzul, OUTPUT);
}

long lerDistancia() {
  digitalWrite(TRIGGER_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN, LOW);
  
  long duracao = pulseIn(ECHO_PIN, HIGH);
  long distancia = duracao * 349.24 / 2 / 10000;
  
  return distancia;
}

void loop() {
  
  int leituraLDR = analogRead(LDR_PIN);
  float tensao = (leituraLDR * 3.3) / 4095.0;
  
  Serial.print("Leitura LDR: ");
  Serial.print(leituraLDR);
  Serial.print(" - Tensão: ");
  Serial.println(tensao);
  
  if (leituraLDR < 3000) {
    mqtt.publish(presenca_1,"acender");
  } else {
    mqtt.publish(presenca_1,"apagar");
  }

  
  long distancia = lerDistancia();
  
  Serial.print("Distância: ");
  Serial.print(distancia);
  Serial.println(" cm");
  
  if (distancia < 10) {
    mqtt.publish(presenca_1,"detectado");
  }

  float umidade = dht.readHumidity(){
    mqtt.publish(umidade, "detectado");
  }
  float temperatura = dht.readTemperature(){
    mqtt.publish(temperatura, "detectado");
  }  
  
  if (isnan(umidade) || isnan(temperatura)) {
    Serial.println("Erro na leitura do DHT11");
    return;
  }
  
  Serial.print("Umidade: ");
  Serial.print(umidade);
  Serial.print("%  Temperatura: ");
  Serial.print(temperatura);
  Serial.println("°C");

  mqtt.loop();
  delay(10);
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








