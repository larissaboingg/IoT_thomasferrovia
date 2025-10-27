#include <WiFi.h>
#include <PubSubClient.h>

WiFiClient client;        //cria cliente wifi
PubSubClient mqtt(client);  //fala que mqtt usa o cliente wifi

const String SSID = "FIESC_IOT_EDU";
const String PASS = "8120gv08";

const int PORT           =1883;
const String URL         ="test.mosquitto.org";

const String broker_user ="";
const String broker_pass = "";

const String MyTopic    ="Et_topico"; //define da onde vou receber as msgs
const String OtherTopic ="Lk_topico"; //define da onde vou enviar as msgs

const int ledPin = 2;

void setup() {
  pinMode(ledPin, OUTPUT);
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
  mqtt.subscribe(MyTopic.c_str());
  mqtt.setCallback(callback);
  Serial.println("\nConectado ao broker com sucesso!");
}

void loop() {
  String mensagem ="";
  if(Serial.available()>0){
  mensagem += Serial.readStringUntil('\n'); // le mensagens que o usuário digitou 
  mqtt.publish(OtherTopic.c_str(), mensagem.c_str()); // envia a msg para o topico do colega
  }
  mqtt.loop();
}

void callback(char* topic, byte* payload, unsigned int length){
  String mensagem = "";
  for(int i = 0; i < length; i++){
    mensagem += (char)payload[i];
  }
  Serial.print("Recebido: ");
  Serial.println(mensagem);
  if(mensagem == "Acender"){
    digitalWrite(ledPin, HIGH);
  } else if (mensagem == "Apagar"){
    digitalWrite(ledPin, LOW);
  }
}