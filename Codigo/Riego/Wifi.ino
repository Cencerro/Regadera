void conectarWifi() {
  Serial.println();
  Serial.print("Conectado a la Wifi");
  WiFi.begin("nombre", "pwd");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print(WiFi.localIP());
  Serial.println("");
}
/*
  void sincNtp(NTPClient cliHora) {
  Serial.println();
  Serial.println("Sincronizando la Hora por NTP");
  cliHora.update();
  Serial.println(cliHora.getFormattedTime());
  Serial.println();
  }*/
