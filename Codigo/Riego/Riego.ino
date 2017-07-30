// Programa para manejar bomba de agua para regar
// 22 Feb 2017
//
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <PubSubClient.h>


// Variables y constantes
byte ciclo = 5; // repeticiones del riego en una sesion. 
word tpoRiego = 12000; // tiempo que mantenemos la bomba encendida
unsigned long tpoPausa = 300000; // Tiempo de pausa entre riegos para dejar que la tierra absorba el agua.
unsigned int horaRiego = 1900;  // HHMM
unsigned long espera = 1; // tiempo restante hasta el siguiente evento.
WiFiUDP ntpUDP;
NTPClient clienteNTP(ntpUDP);
int p_bomba = 15; //Pin para control de la bomba de agua.
int dia, hora, minuto, segundo = 0;

// Para Mosquitto
  const char mqtt_srv[] = "192.168.1.101";  //Servidor Mosquitto
  const char tag_log[] = "Regadera/Log";        //Cola para enviar informacion de log
  const char tag_riego[] = "Regadera/Riego";        //Cola para enviar datos de riego
  String texto; // Para enviar textos al mqtt

 
void setup() {
  Serial.begin(115200);
  Serial.println("---------------- INICIO ----------------");
  conectarWifi();
  clienteNTP.begin();
  texto += clienteNTP.getFormattedTime();
  texto += " ; Empezamos"; 
  enviarMQTT (mqtt_srv, tag_log, texto.c_str());
  pinMode (p_bomba, OUTPUT);
  Serial.println("FIN del setup");
}

void loop() {
  Serial.println("Inicio del loop");
  Serial.print(WiFi.localIP());  Serial.println("");
  clienteNTP.update();
  Serial.println(clienteNTP.getFormattedTime());
  
// Tomamos la hora y la añadimos a texto como encabezado para el log por mqtt
  leerHora(dia, hora, minuto, segundo);
  texto = String(dia);  texto += "-";
  texto += String(hora);  texto += ":";
  texto += String(minuto);  texto += ":";
  texto += String(segundo);  texto += " -> ";
  Serial.print("Hora fuera de la funcion: ");  Serial.println(texto);
  Serial.println(hora*10000+ minuto*100+ segundo);
  // Falta comprobar mandar esperar hasta el sigiente dia.
  
  while (espera != 0) {
    espera = siguiente(horaRiego);
    texto += String(espera);
    enviarMQTT (mqtt_srv, tag_log, texto.c_str());
    Serial.println(texto);
    dormir (espera);    
    texto = String(clienteNTP.getFormattedTime());
    texto += String(" -> Espera terminada");
    enviarMQTT (mqtt_srv, tag_log, texto.c_str());
  }
  // Tomamos la hora y la añadimos a texto como encabezado para el log por mqtt
  leerHora(dia, hora, minuto, segundo);
  texto = String(dia);  texto += "-";
  texto += String(hora);  texto += ":";
  texto += String(minuto);  texto += ":";
  texto += String(segundo);  texto += " -> ";
  texto += "Empezamos a regar";
  enviarMQTT (mqtt_srv, tag_log, texto.c_str());

  cilcoRiego();
  
  espera = siguiente((hora+1)*100);
  dormir(espera);
  Serial.println();
}

