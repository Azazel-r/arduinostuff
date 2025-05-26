/*  
  Rui Santos & Sara Santos - Random Nerd Tutorials
  https://RandomNerdTutorials.com/esp32-web-server-beginners-guide/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/

// Replace with your network credentials
const char* ssid = "FRITZ!Box 6660 Cable PD";
const char* password = "32157818529049484647";

// Function to handle setting the brightness
void handleSet() {
  if (server.hasArg("value")) {
    brightness = server.arg("value").toInt();
    Serial.print("Got a value: ");
    Serial.println(brightness);
  }
  handleRoot();
}

// Function to handle the root URL and show the current states
void handleRoot() {

  // header with style
  String html = "<!DOCTYPE html><html lang=\"en\"><head>";
  html += "<meta charset=\"UTF-8\">";
  html += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">";
  html += "<title>ESP LED Test</title>";
  html += "<style> .slidecontainer { width: 500px; }";
  html += ".slider { -webkit-appearance: none; width: 100%; height: 15px; border-radius: 5px; background: #d3d3d3; outline: none; opacity: 0.7; -webkit-transition: .2s; transition: opacity .2s; }";
  html += ".slider::-webkit-slider-thumb { -webkit-appearance: none; appearance: none; width: 25px; height: 25px; border-radius: 50%;  background: #04AA6D; cursor: pointer; }";
  html += ".slider::-moz-range-thumb { width: 25px; height: 25px; border-radius: 50%; background: #04AA6D; cursor: pointer; }";
  html += ".myButton { width: 100px; height: 50px; } </style>";
  html += "</head><body>";

  // body with headline and slider and button
  html += "<h1><p>This slider does something</p></h1>";
  html += "<div class=\"slidecontainer\"><input type=\"range\" min=\"0\" max=\"255\" value=\"127\" class=\"slider\" id=\"myRange\"></div>";
  html += "<button onclick=\"goToPage()\" class=\"myButton\">Go!</button>";
  html += "<script> const slider = document.getElementById(\"myRange\"); function goToPage() { const value = slider.value; window.location.href = `/set?value=${value}`; }</script>";
  html += "</body></html>";

  server.send(200, "text/html", html);
}

void setupServer() {
  // Connect to Wi-Fi network
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Set up the web server to handle different routes
  server.on("/", handleRoot);
  server.on("/set", handleSet);

  // Start the web server
  server.begin();
  Serial.println("HTTP server started");
}

