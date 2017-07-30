void cilcoRiego(){
    for (int i=1; i <= ciclo;  i++){
    digitalWrite(p_bomba, HIGH);
    Serial.print("Bombeando ciclo ");
    Serial.print(i);
    Serial.print("   -   tiempo: ");
    Serial.println(tpoRiego);
    delay(tpoRiego);
    digitalWrite(p_bomba, LOW);
    Serial.print("Dormimos: ");
    Serial.println(tpoPausa);
    delay(tpoPausa);
  }
  Serial.println("Fin del ciclo ----------");
}

/* 
 *  Calculamos tiempo hasta el siguiente evento y lo retornamos en segundos
 *  
*/
unsigned long siguiente(unsigned int sigEvento){  // Calculamos tiempo para siguiente evento
  unsigned long retorno = 60000; // tiempo maximo en segundos ya que cada hora comprobamos algo
  int minRiego = sigEvento % 100;
  Serial.print("Minuto Riego: "); Serial.println(minRiego);
  leerHora(dia, hora, minuto, segundo);
  if ((hora == sigEvento / 100) && ((dia % 2) != 0)){ //regamos L-X-V--
    long resta = (sigEvento % 100) - minRiego;
    if ((resta < 1) && (resta >= 0)){ // menos de un minuto, retornamos los segundos que faltan.
      Serial.print("Menos de un minuto. Fataban (mn): "); Serial.println(resta);
      retorno = 59-segundo;
    } else {
        if (resta < 0) {  // es en la siguiente hora
          retorno = ((59 - minRiego) * 100)+ (59 - segundo);
        } else {          // es en esta hora
          retorno = (minRiego - minuto - 1) * 100 + (50 - segundo);  
        }
    }
  } else { // sigiente evento es el la hora proxima
    retorno = (59 - minuto) * 100 + 59 -segundo;
  }
  return (retorno);
}


/* 
 *  Pausamos hasta el siguiente riego
 *  Maximo deep sleep son 4294967295 microsegundos. (unos 71mn).
 *  Vamos a dormir una hora (3600000000us) para permitir comprobaciones horarias en el futuro 
*/
      
void dormir (unsigned long tiempo){ 
    Serial.print("Dormir. tiempo = "); Serial.println(tiempo);
    texto = String(clienteNTP.getFormattedTime());
    if (tiempo < 300){ //5mn
        delay (tiempo*1000);
        texto += String(" -> Sueño ligero de segundos: ");
        texto += String(tiempo);
        enviarMQTT (mqtt_srv, tag_log, texto.c_str());
      } else {
        Serial.println("Apagamos led y a Dormir profundo");
        digitalWrite(5, LOW); //apagamos el led para ahorrar
        texto += String(" -> Sueño profundo de segundos: ");
        texto += String(tiempo);
        enviarMQTT (mqtt_srv, tag_log, texto.c_str());
//      delay(30000); // durante el deepsleep no falla la carga. Activar durante las pruebas..
//    ESP.deepSleep(3600000000); // una hora en micro segundos
// Para que despierte los pines RST y GPIO16 (XPD_DCDC o pin8)tienen que estar conectados.
        ESP.deepSleep(tiempo*1000000);
  }
Serial.println("retornamos de la fn dormir, es hora de hacer algo");
}

void leerHora(int &dia, int &hora, int &minuto, int &segundo){
  clienteNTP.update();
  dia = clienteNTP.getDay();
  hora = clienteNTP.getHours();
  minuto =  clienteNTP.getMinutes();
  segundo = clienteNTP.getSeconds();
  Serial.print("Hora en la funcion:       ");
  Serial.print(dia);  Serial.print("-");
  Serial.print(hora);  Serial.print(":");
  Serial.print(minuto);  Serial.print(":");
  Serial.println(segundo);
  return;
}

/*
 * Funciones para comunicar con servidor Mosquitto
 */

 void enviarMQTT(const char* mqtt_srv, const char* cola, const char* lectura){ 
  WiFiClient clienteWifi;
  byte fallos = 0;
  //Nos conectamos a mqtt
  PubSubClient client(clienteWifi);
  client.setServer(mqtt_srv, 1883);
  while (client.state() != 0 && fallos < 10) {
    Serial.println("conectando a MQTT...");
    Serial.println(client.state());
    if (client.connect("Regadera")) {
        Serial.println("conectado a MQTT!");
        client.publish(cola, lectura);
    } else {
      fallos++;
      Serial.print("Fallo, rc=");
      Serial.print(client.state());
     }
  }
}

char* string2char(String cadena){
    if(cadena.length()!=0){
        char *resultado = const_cast<char*>(cadena.c_str());
        return resultado;
    }
}



