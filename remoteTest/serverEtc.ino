/*  
  Rui Santos & Sara Santos - Random Nerd Tutorials
  https://RandomNerdTutorials.com/esp32-web-server-beginners-guide/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/

// Replace with your network credentials
const char* ssid = "Azalea's Garden"; // "Lars stinkt <3"; // "FRITZ!Box 6660 Cable PD"; // 
const char* password = "12345678"; // "fabi1234"; // "Pommes1+"; // "32157818529049484647"; // 

// Function to handle setting the brightness
void handleSet() {
  if (server.hasArg("value")) {
    brightness = (int) mapf(server.arg("value").toInt(), 0, 100, 0, 255);
    Serial.print("Got a value: ");
    Serial.println(brightness);
  }
  handleRoot();
}

void handleAngle() {
  if (server.hasArg("value")) {
    // activeAngle = (int) mapf(server.arg("value").toInt(), 0, 100, 0, 90);
    activeAngle = server.arg("value") == "min" ? 0 : 140;
    Serial.print("Got an angle: ");
    Serial.println(activeAngle);
  }
  handleRoot();
}

void handleHue() {
  if (server.hasArg("value")) {
    activeColValue = server.arg("value").toInt();
    Serial.print("Got a HUE Value: ");
    Serial.println(activeColValue);
  }
  handleRoot();
}

void handleColor() {
  if (server.hasArg("c")) {
    activeFunction = server.arg("c").toInt();
    Serial.print("Got a color: ");
    Serial.println(activeFunction);
  }
  handleRoot();
}

void handleStartup() {
  startFramesForAnim = frameCount;
  activeFunction = -3;
  Serial.print("Starting Animation at frame: ");
  Serial.println(frameCount);
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
  html += ".rb { background-color: #FF0000; } .yb { background-color: #FFFF00; } .gb {background-color: #00FF00; } .wb { background-color: #FFFFFF; }";
  html += ".myButton { width: 100px; height: 50px; } </style>";
  html += "</head><body>";

  // body with headline and slider and button
  html += "<h1><p>Slider = Brightness</p></h1>";
  html += "<div class=\"slidecontainer\"><input type=\"range\" min=\"0\" max=\"100\" value=\"50\" class=\"slider\" id=\"myRange\"></div>";
  html += "<button onclick=\"brightness()\" class=\"myButton\">Brightness</button>";
  html += "<script> const slider = document.getElementById(\"myRange\"); function brightness() { const value = slider.value; window.location.href = `/set?value=${value}`; }</script>";
  html += "<button onclick=\"angleMin()\" class=\"myButton\">Servo Min</button>";
  html += "<script> function angleMin() { window.location.href = \"/angle?value=min\"; }</script>";
  html += "<button onclick=\"angleMax()\" class=\"myButton\">Servo Max</button>";
  html += "<script> function angleMax() { window.location.href = \"/angle?value=max\"; }</script>";
  // html += "<script> const slider2 = document.getElementById(\"myRange\"); function angle() { const value = slider.value; window.location.href = `/angle?value=${value}`; }</script>";
  html += "<button onclick=\"rainbow()\" class=\"myButton wb\">Rainbow!</button>";
  html += "<button onclick=\"solidGreen()\" class=\"myButton gb\">Green</button>";
  html += "<button onclick=\"solidYellow()\" class=\"myButton yb\">Yellow</button>";
  html += "<button onclick=\"solidRed()\" class=\"myButton rb\">Red</button>";
  html += "<button onclick=\"startup()\" class=\"myButton wb\">Startup</button>";
  html += "<script> function rainbow() { window.location.href = \"/color?c=0\"; }</script>"; // rainbw
  html += "<script> function solidGreen() { window.location.href = \"/color?c=1\"; }</script>"; // G
  html += "<script> function solidYellow() { window.location.href = \"/color?c=2\"; }</script>"; // Y
  html += "<script> function solidRed() { window.location.href = \"/color?c=3\"; }</script>"; // R
  html += "<script> function startup() { window.location.href = \"/startup\"; }</script>"; // startup
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
  server.on("/color", handleColor);
  server.on("/setHue", handleHue);
  server.on("/startup", handleStartup);
  server.on("/angle", handleAngle);

  // Start the web server
  server.begin();
  Serial.println("HTTP server started");
}

