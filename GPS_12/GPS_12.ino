#include <Wire.h>
#include <WiFi.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include <TinyGPS++.h>
#include <HardwareSerial.h>
#include <Servo.h>

#define RXD2 16
#define TXD2 17

#define BOT_TOKEN "YOUR_BOT_TOKEN"  // Reemplaza con el token de tu bot
#define CHAT_ID "YOUR_CHAT_ID"      // Reemplaza con tu ID de chat de Telegram

#define NMEA 0

float targetLat = 0.0;
float targetLon = 0.0;
const char* ssid = "Ale";
const char* password = "alejaaaa";

int VelocidadMotor1 = 5;
int VelocidadMotor2 = 18;  // Cambiado a un nuevo pin

int echoPin = 2;
int trigPin = 15;  // Cambiado a un nuevo pin
long duration;
int distance;
int delayVal;

// Definición de los pines de control de giro de los motores In1, In2, In3 e In4
int Motor1A = 13;
int Motor1B = 12;
int Motor2C = 14;  // Cambiado a un nuevo pin
int Motor2D = 27;  // Cambiado a un nuevo pin
// Variable control posición servo y observaciones
int servoPos = 0;
int servoReadLeft = 0;
int servoReadRight = 0;

char   datoCmd = 0;

UniversalTelegramBot bot(BOT_TOKEN, Serial);
SoftwareSerial gpsSerial(RXD2, TXD2); // RX, T
HardwareSerial neogps(1);

TinyGPSPlus gps;
Servo myservo;

void setup() {
  
  WiFi.begin(ssid, password);
  delay(1000);
  
  Serial.begin(115200);
  neogps.begin(9600, SERIAL_8N1, RXD2, TXD2); 
  myservo.attach(21);
  // Establecemos modo de los pines del sensor de ultrasonidos
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  // Establecemos modo de los pines del control de motores
  pinMode(Motor1A,OUTPUT);
  pinMode(Motor1B,OUTPUT);
  pinMode(Motor2C,OUTPUT);
  pinMode(Motor2D,OUTPUT);
  pinMode(VelocidadMotor1, OUTPUT);
  pinMode(VelocidadMotor2, OUTPUT);
  // Configuramos velocidad de los dos motores
  analogWrite(VelocidadMotor1, 75);
  analogWrite(VelocidadMotor2, 80);
  myservo.write(90);
  
}

void loop() {
// Manejo de Telegram
  bot.handleLongPoll(20);
  if (NMEA){
    while (neogps.available())
    {
     datoCmd  = (char)neogps.read(); 
     Serial.print (datoCmd );
    }
 }else{
  boolean newData = false;
  for (unsigned long start = millis(); millis() - start < 1000;){
    while (neogps.available()){
      if (gps.encode(neogps.read())){
        newData = true;         
      }
    }
  }  

  if(newData == true){
    newData = false;
    Serial.println(gps.satellites.value());    
    Visualizacion_Serial();
  }else{
  }  
 }

  // Manejo de GPS
  while (gpsSerial.available() > 0) {
    if (gps.encode(gpsSerial.read())) {
      // Obtener la ubicación
      float currentLat = gps.location.lat();
      float currentLon = gps.location.lng();
      // Lógica de movimiento hacia la ubicación
      moveToLocation(targetLat, targetLon);
    }
  }

  distance = medirDistancia();
  Serial.println(distance);
  if(distance < 15){
    stopCar();
    // Miramos a la derecha
    myservo.write(10);
    delay(600);
    servoReadRight = medirDistancia();
    // Miramos a la izquierda
    myservo.write(170);
    delay(600);
    servoReadLeft = medirDistancia();
    // Miramos de frente
    myservo.write(90);
    delay(600);
    if(servoReadLeft > servoReadRight){
    Serial.println("Giro izquierda");
    turnLeftCar();
    }
    if(servoReadRight >= servoReadLeft){
    Serial.println("Giro derecha");
    turnRightCar();
    }
  }
  if(distance > 15){
  Serial.println("Recto");
  moveForwardCar();
  } 
}

void moveToLocation(float targetLat, float targetLon) {
// Lógica de movimiento hacia la ubicación basándote en la diferencia de latitud y longitud
  float deltaLat = targetLat - currentLat;
  float deltaLon = targetLon - currentLon;

  // Calcula el ángulo en radianes
  float angle = atan2(deltaLon, deltaLat);

  // Convierte el ángulo a grados
  float angleDegrees = angle * 180.0 / PI;

  // Ajusta la dirección basándote en el ángulo
  // Por ejemplo, si estás utilizando un controlador de motor, puedes cambiar la velocidad de los motores
  // en función del ángulo para dirigir el carro hacia la ubicación objetivo.
}

void handleNewMessages(int numNewMessages) {
  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    String text = bot.messages[i].text;

    if (text.startsWith("/movetolocation")) {
      // Extraer la latitud y longitud del mensaje
      int separatorIndex = text.indexOf(",");
      if (separatorIndex != -1) {
        String latString = text.substring(text.indexOf(" ") + 1, separatorIndex);
        String lonString = text.substring(separatorIndex + 1);
        // Convertir las cadenas a valores flotantes y almacenar en variables globales
        targetLat = latString.toFloat();
        targetLon = lonString.toFloat();
      }
    }
  }
}

int medirDistancia(){
// Lanzamos pulso de sonido
digitalWrite(trigPin, LOW);
delayMicroseconds(2);
digitalWrite(trigPin, HIGH);
delayMicroseconds(10);
digitalWrite(trigPin, LOW);
// Leemos lo que tarda el pulso en llegar al sensor y calculamos distancia
duration = pulseIn(echoPin, HIGH);
distance = duration * 0.034 / 2;
// Devolver distancia calculada
return distance;
}

void stopCar(){
// Paramos el carrito
digitalWrite(Motor1A, LOW);
digitalWrite(Motor2D, LOW);
digitalWrite(Motor1A, LOW);
digitalWrite(Motor2D,LOW);
}

void turnRightCar(){
// Configuramos sentido de giro para dirar a la derecha
digitalWrite(Motor1A, LOW);
digitalWrite(Motor2D,LOW);
digitalWrite(Motor1A, HIGH);
digitalWrite(Motor2D,LOW);
delay(250);
}

void turnLeftCar(){
// Configuramos sentido de giro para dirar a la izquierda
digitalWrite(Motor1A,LOW);
digitalWrite(Motor2D, LOW);
digitalWrite(Motor1A,LOW);
digitalWrite(Motor2D, HIGH);
delay(250);
}

void moveForwardCar(){
// Configuramos sentido de giro para avanzar
digitalWrite(Motor1A, LOW);
digitalWrite(Motor2D, LOW);
digitalWrite(Motor1A, HIGH);
digitalWrite(Motor2D,HIGH);
}

void Visualizacion_Serial(void){ 
  if (gps.location.isValid() ==  1){  
    Serial.print("Lat: ");
    Serial.println(gps.location.lat(),6);
    Serial.print("Lng: ");
    Serial.println(gps.location.lng(),6);  
    Serial.print("Speed: ");
    Serial.println(gps.speed.kmph());    
    Serial.print("SAT:");
    Serial.println(gps.satellites.value());
    Serial.print("ALT:");   
    Serial.println(gps.altitude.meters(), 0);     

    Serial.print("Date: ");
    Serial.print(gps.date.day()); Serial.print("/");
    Serial.print(gps.date.month()); Serial.print("/");
    Serial.println(gps.date.year());

    Serial.print("Hour: ");
    Serial.print(gps.time.hour()); Serial.print(":");
    Serial.print(gps.time.minute()); Serial.print(":");
    Serial.println(gps.time.second());
    Serial.println("---------------------------");
  }else{
    Serial.println("Sin señal gps");  
  }  
}