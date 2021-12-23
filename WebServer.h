#ifndef _WEBSERVER_H
#define _WEBSERVER_H

#include <Arduino.h>
#include <WebSocketsServer.h>
#include <ESP8266WebServer.h>

class WebServer
{
  public:
    WebServer();
    bool setup();
    bool loop();
    void onRequestStatus(const std::function<String(void)>& f);

  protected:
    std::function<String(void)> onRequestStatusCallback;
    
};

#endif
