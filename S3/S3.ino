#include <WiFi.h>
#include <PubSubClient.h>
#include <ESP32Servo.h>

WiFiClient client;        //cria cliente wifi
PubSubClient mqtt(client);  //fala que mqtt usa o cliente wifi

const String SSID = "FIESC_IOT_EDU";
const String PASS = "8120gv08";

const int PORT           =8883;
const String URL         ="c2388c77116243a0a57bd57d29c9b376.s1.eu.hivemq.cloud";

const String broker_user ="Sensor3";
const String broker_pass = "Eduarda123";

//envia/recebe
const char* presenca_2 = "presenca2";
const char* presenca_3 = "presenca3";
const char* presenca_4 = "presenca4";
const char* iluminacao = "iluminacao";

//envia
const char* servo_1 = "servo1";
const char* servo_2 = "servo2";

byte atual = 90;

const int ledPin = 2;
const byte TRIGGER_PIN = 5;
const byte ECHO_PIN = 12;
Servo meuServo;
const byte Servo1 = 18;
const byte Servo2 = 19;

void setup() {

  pinMode(ledPin, OUTPUT);
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  meuServo.attach(Servo1);
  meuServo.attach(Servo2);
  
  Serial.begin(115200);
  Serial.println("Conectando ao Wifi");
  WiFi.begin(SSID,PASS);
  while(WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(200);
  }
  Serial.println("\nConectado!");
  Serial.println("Conectando ao broker...");
  mqtt.setServer(URL.c_str(),PORT);
  while(!mqtt.connected()){
    String ID = "S3-";
    ID += String(random(0xffff),HEX);
    mqtt.connect(ID.c_str(),broker_user.c_str(),broker_pass.c_str());
    delay(200);
    Serial.print(".");
  }
  
  mqtt.subscribe(presenca_2);
  mqtt.subscribe(presenca_3);
  mqtt.subscribe(presenca_4);
  mqtt.subscribe(iluminacao);

  mqtt.setCallback(callback);
  Serial.println("\nConectado ao broker com sucesso!");
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

//Dentro do loop vao os envios de informações
void loop() {
  //                            1º Fazer ler o ultrassonico e publicar no topico 4 quando detectar < 10
  long distancia = lerDistancia();
  
  Serial.print("Distância: ");
  Serial.print(distancia);
  Serial.println(" cm");
  
  if (distancia < 10) {
    mqtt.publish(presenca_4,"detectado");
  }
  mqtt.loop();
  delay(10);  
}


//Dentro do callback os recebimentos das informações,
void callback(char* topic, byte* payload, unsigned int length){
  String mensagem = "";
  for(int i = 0; i < length; i++){
    mensagem += (char)payload[i];
  }
  Serial.print("Recebido: ");
  Serial.println(mensagem);


  if(topic == presenca_4 && mensagem == "detectado"){
    if(atual == 90){
      servo1.write(120);
      atual = 120;
    }
    else{
      servo1.write(90);
      atual = 90;
    }
  }
  else if (topic == presenca_2 && mensagem == "detectado"){
    servo1.write(90);
  }
  else if (topic == presenca_3 && mensagem == "detectado"){
    servo2.write(120);
  }
  else if (topic == iluminacao && mensagem == "acender"){
    digitalWrite(ledPin, HIGH);
  }
  else if (topic == iluminacao && mensagem == "apagar"){
    digitalWrite(ledPin, LOW);
  }
}








