#include <WiFiClientSecure.h>
#include <PubSubClient.h>

WiFiClientSecure client;    //cria cliente wifi
PubSubClient mqtt(client);  //fala que mqtt usa o cliente wifi

const String SSID = "FIESC_IOT_EDU";
const String PASS = "8120gv08";

const int PORT = 8883;
const String URL = "c2388c77116243a0a57bd57d29c9b376.s1.eu.hivemq.cloud";

const String broker_user = "Sensor2";
const String broker_pass = "Larissa123";

//recebe
const char* iluminacao = "iluminacao";

//envia
const char* presenca_2 = "presenca2";
const char* presenca_3 = "presenca3";

const int ledPin = 2;
const byte TRIGGER_1 = 5;
const byte ECHO_1 = 12;

const byte TRIGGER_2 = 6;
const byte ECHO_2 = 13;

void setup() {
  Serial.begin(115200);

  pinMode(ledPin, OUTPUT);
  pinMode(TRIGGER_1, OUTPUT);
  pinMode(ECHO_1, INPUT);
  pinMode(TRIGGER_2, OUTPUT);
  pinMode(ECHO_2, INPUT);

  Serial.println("Conectando ao Wifi");
  WiFi.begin(SSID, PASS);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(200);
  }
  Serial.println("\nConectado!");
  client.setInsecure();
  Serial.println("Conectando ao broker...");
  mqtt.setServer(URL.c_str(), PORT);
  while (!mqtt.connected()) {
    String ID = "S2-";
    ID += String(random(0xffff), HEX);
    mqtt.connect(ID.c_str(), broker_user.c_str(), broker_pass.c_str());
    delay(200);
    Serial.print(".");
  }

  mqtt.subscribe(presenca_2);
  mqtt.subscribe(presenca_3);
  mqtt.subscribe(iluminacao);

  mqtt.setCallback(callback);
  Serial.println("\nConectado ao broker com sucesso!");
}

long lerDistancia1() {
  digitalWrite(TRIGGER_1, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGGER_1, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_1, LOW);

  long duracao1 = pulseIn(ECHO_1, HIGH);
  long distancia1 = duracao1 * 349.24 / 2 / 10000;

  return distancia1;
}

long lerDistancia2() {
  digitalWrite(TRIGGER_2, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGGER_2, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_2, LOW);

  long duracao2 = pulseIn(ECHO_2, HIGH);
  long distancia2 = duracao2 * 349.24 / 2 / 10000;

  return distancia2;
}

void loop() {
  long distancia1 = lerDistancia1();

  Serial.print("Distância: ");
  Serial.print(distancia1);
  Serial.println(" cm");

  if (distancia1 < 10) {
    mqtt.publish(presenca_2, "detectado");
  }
  long distancia2 = lerDistancia2();

  Serial.print("Distância: ");
  Serial.print(distancia2);
  Serial.println(" cm");

  if (distancia2 < 10) {
    mqtt.publish(presenca_3, "detectado");
  }

  mqtt.loop();
  delay(10);
}

void callback(char* topic, byte* payload, unsigned int length) {
  String mensagem = "";
  for (int i = 0; i < length; i++) {
    mensagem += (char)payload[i];
  }
  Serial.print("Recebido: ");
  Serial.println(mensagem);
  else if (topic == iluminacao && mensagem == "acender") {
    digitalWrite(ledPin, HIGH);
  }
  else if (topic == iluminacao && mensagem == "apagar") {
    digitalWrite(ledPin, LOW);
  }
}