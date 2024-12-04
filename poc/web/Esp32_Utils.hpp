// Guardar las credenciales
void handleSave() {
  String ssid = server.arg("ssid");
  String password = server.arg("password");

  if (ssid.isEmpty() || password.isEmpty()) {
    server.send(400, "text/plain", "Por favor, completa ambos campos.");
    return;
  }

  // Guardar en memoria no volátil
  preferences.begin("wifi", false);
  preferences.putString("ssid", ssid);
  preferences.putString("password", password);
  preferences.end();

  server.send(200, "text/html", "Credenciales guardadas. Reinicia el dispositivo.");
}

// Intentar conectarse a WiFi
bool connectToWiFi() {
  preferences.begin("wifi", true);
  String storedSSID = preferences.getString("ssid", "");
  String storedPassword = preferences.getString("password", "");
  preferences.end();

  if (storedSSID.isEmpty() || storedPassword.isEmpty()) {
    Serial.println("No hay credenciales almacenadas.");
    return false;
  }

  Serial.print("Intentando conectar a WiFi: ");
  Serial.println(storedSSID);

  WiFi.begin(storedSSID.c_str(), storedPassword.c_str());
  for (int i = 0; i < 20; i++) {  // Intentar durante 10 segundos
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("Conectado a WiFi.");
      Serial.print("Dirección IP: ");
      Serial.println(WiFi.localIP());
      return true;
    }
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nNo se pudo conectar al WiFi.");
  return false;
}

// Intentar conectarse al WiFi almacenado
void conectarseAWifi(){
    if (!connectToWiFi()) {
    // Si falla, iniciar modo punto de acceso
    Serial.println("Iniciando modo AP...");
    WiFi.softAP(apSSID, apPassword);
    Serial.print("IP del AP: ");
    Serial.println(WiFi.softAPIP());

    // Configurar servidor web
    server.on("/", handleRoot);
    server.on("/save", HTTP_POST, handleSave);
    server.begin();
    Serial.println("Servidor web iniciado.");
  }
}
  