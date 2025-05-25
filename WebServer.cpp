#include <Arduino.h>
#include <FrameRateCounter.h>
#include <FS.h>
#include <ESP8266WiFi.h>
#include <WebSocketsServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include "WebServer.h"
#include "catGlb.h"

static const char WIFI_SSID[] = "Middlemansion";
static const char WIFI_PASS[]= "kungcharlie";
static const char MDNS_NAME[]= "pomodorokitten";


ESP8266WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);
FrameRateCounter xSendTimer(20);

WebServer::WebServer() {
  
}

bool WebServer::setup() {
  WiFi.disconnect();
  WiFi.softAPdisconnect(true);
  WiFi.mode(WIFI_STA);
  // Connect to a WiFi network
  Serial.print(F("Connecting to "));  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID,WIFI_PASS);

  // connection with timeout
  int count = 0; 
  while ( (WiFi.status() != WL_CONNECTED) && count < 17) 
  {
      Serial.print(".");  delay(500);  count++;
  }
 
  if (WiFi.status() != WL_CONNECTED)
  { 
     Serial.println("");  Serial.print("Failed to connect to ");  Serial.println(WIFI_SSID);
     while(1);
  }
 
  Serial.println("");
  Serial.println(F("[CONNECTED]"));   Serial.print("[IP ");  Serial.print(WiFi.localIP()); 
  Serial.println("]");
 
  if (!MDNS.begin(MDNS_NAME)) {
    Serial.println("Error setting up MDNS responder!");
  }
  Serial.println("mDNS responder started");
  
  // start a server
  server.on("/", [&]() {
    //File file = SPIFFS.open("/index.html", "r");                 // Open it
    //size_t sent = server.streamFile(file, "text/html"); // And send it to the client
    //file.close();
    server.send(200, "text/html", "<!DOCTYPE html><html> <head> <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"/> <meta charset=\"utf-8\"/> <style>body{background-color: black;}#time{color: #fff; font-family: -apple-system, BlinkMacSystemFont, \"Segoe UI\", Roboto, Oxygen, Ubuntu, Cantarell, \"Open Sans\", \"Helvetica Neue\", sans-serif; text-align: center; font-size: 8rem; margin: 0; transition: all 1s; transition-timing-function: ease-in-out;}#state{display: block; color: #fff; font-family: -apple-system, BlinkMacSystemFont, \"Segoe UI\", Roboto, Oxygen, Ubuntu, Cantarell, \"Open Sans\", \"Helvetica Neue\", sans-serif; text-align: center; font-size: 2rem; margin-top: 20%; transition: all 1s; transition-timing-function: ease-in-out;}#cat{width: 200px; height: 200px; background-color: red; transition: transform 0.05s;}</style> <style>body{overflow: hidden;}model-viewer{--poster-color: transparent; width: 100%; height: 500px;}</style> <script type=\"module\" src=\"https://unpkg.com/@google/model-viewer/dist/model-viewer.min.js\" ></script> <title>Pomodoro Kitten</title> </head> <body> <div id=\"main\"> <span id=\"state\"></span> <h1 id=\"time\"></h1> <model-viewer interpolation-decay=\"40\" id=\"model-viewer\" alt=\"Cat\" src=\"cat.glb\"></model-viewer> </div></body> <script>var Socket; var bodyDOM=document.body; var timeDOM=document.getElementById(\"time\"); var catDOM=document.getElementById(\"cat\"); var stateDOM=document.getElementById(\"state\"); var modelViewer=document.getElementById(\"model-viewer\"); function init(){Socket=new WebSocket(\"ws://\"+location.hostname+\":81/\"); Socket.onmessage=function (event){processReceivedCommand(event);};}function processReceivedCommand(evt){let args=evt.data.split(\",\"); let x=args[0]; let timeLeft=args[1]; let pomodoroState=args[2]; let formattedTimeLeft=fancyTimeFormat(timeLeft / 1000); console.log(x); timeDOM.innerHTML=formattedTimeLeft; stateDOM.innerHTML=pomodoroState; if(pomodoroState==\"SLEEP\"){timeDOM.style.height=\"0px\"; stateDOM.style.height=\"0px\"; timeDOM.style.opacity=\"0\"; stateDOM.style.opacity=\"0\";}else{timeDOM.style.height=\"160px\"; stateDOM.style.height=\"40px\"; timeDOM.style.opacity=\"1\"; stateDOM.style.opacity=\"1\";}if(pomodoroState==\"SHORT BREAK\" || pomodoroState==\"LONG BREAK\"){bodyDOM.style.background=\"#00B454\";}else{bodyDOM.style.background=\"black\";}modelViewer.cameraOrbit=`0deg ${(x - 500) * -1}deg 4m`;}function fancyTimeFormat(duration){var hrs=~~(duration / 3600); var mins=~~((duration % 3600) / 60); var secs=~~duration % 60; var ret=\"\"; if (hrs > 0){ret +=\"\" + hrs + \":\" + (mins < 10 ? \"0\" : \"\");}ret +=\"\" + mins + \":\" + (secs < 10 ? \"0\" : \"\"); ret +=\"\" + secs; return ret;}function sendText(data){Socket.send(data);}window.onload=function (e){init();}; </script></html>");
  });
  server.on("/cat.glb", [&]() {
    //File file = SPIFFS.open("/cat.glb", "r");                 // Open it
    //size_t sent = server.streamFile(file, "model/gltf-binary"); // And send it to the client
    //file.close();
    server.send_P(200, "model/gltf-binary", catGlb, sizeof(catGlb));
  });
  server.begin();
  Serial.println("Server started");
 
  webSocket.begin();
  webSocket.onEvent([&](byte num, WStype_t type, uint8_t * payload, size_t length)
{
  if(type == WStype_TEXT)
  {
      if (payload[0] == '0')
      {       
      }
      else if (payload[0] == '1')
      {     
      }
  }
 
  else 
  {
    Serial.print("WStype = ");   Serial.println(type);  
    Serial.print("WS payload = ");
    for(int i = 0; i < length; i++) { Serial.print((char) payload[i]); }
    Serial.println();
 
  }
}
);

  xSendTimer.onUpdate([&]() {
    if(onRequestStatusCallback) {
      String statusString = onRequestStatusCallback();
      if(statusString) webSocket.broadcastTXT(statusString);
    }
  });
  xSendTimer.start();
  
  return true;
}

bool WebServer::loop() {
  MDNS.update();
  xSendTimer.update();
  webSocket.loop();

  server.handleClient();
  return true;
}

void WebServer::onRequestStatus(const std::function<String(void)>& f) {
  onRequestStatusCallback = f;
}


void webSocketEvent(byte num, WStype_t type, uint8_t * payload, size_t length)
{
  if(type == WStype_TEXT)
  {
      if (payload[0] == '0')
      {       
      }
      else if (payload[0] == '1')
      {     
      }
  }
 
  else 
  {
    Serial.print("WStype = ");   Serial.println(type);  
    Serial.print("WS payload = ");
    for(int i = 0; i < length; i++) { Serial.print((char) payload[i]); }
    Serial.println();
 
  }
}
