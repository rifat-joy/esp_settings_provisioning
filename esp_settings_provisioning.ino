// #include <WiFi.h>
#include <WebServer.h>
#include <EEPROM.h>

WebServer server(80);

#define FORM "<!doctype html> <html lang='en'> <head> <meta charset='utf-8'> <meta name='viewport' content='width=device-width, initial-scale=1'> <title>RU Setup</title> <style> *, ::after, ::before { box-sizing: border-box; } body { margin: 0; font-family: 'Segoe UI', Roboto, 'Helvetica Neue', Arial, 'Noto Sans', 'Liberation Sans'; font-size: 1rem; font-weight: 400; line-height: 1.5; color: #212529; background-color: #f5f5f5; } .form-control { display: block; width: 100%; height: calc(1.5em + .75rem + 2px); border: 1px solid #ced4da; } button { cursor: pointer; border: 1px solid transparent; color: #fff; background-color: #007bff; border-color: #007bff; padding: .5rem 1rem; font-size: 1.25rem; line-height: 1.5; border-radius: .3rem; width: 100%; } .form-signin { width: 100%; max-width: 400px; padding: 15px; margin: auto; } h1 { text-align: center; } </style> </head> <body> <main class='form-signin'> <form action='/' method='post'> <h1 class=''>RU Settings</h1><br/> <div class='form-floating'> <label>SSID</label> <input type='text' class='form-control' name='ssid'> </div> <div class='form-floating'><br/> <label>Password</label> <input type='text' class='form-control' name='password'> </div> <div class='form-floating'><br/> <label>RU Tag</label> <input type='text' class='form-control' name='ru_tag'> </div> <div class='form-floating'><br/> <label>Ping Topic</label> <input type='text' class='form-control' name='ping_topic'> </div> <div class='form-floating'><br/> <label>Drop sensor ideal status</label> <input type='number' class='form-control' name='status'> </div> <div class='form-floating'><br/> <label>Mag and Spring Configuration</label> <input type='number' class='form-control' name='mag'> </div><br/><br/> <button type='submit'>Save</button> <p style='text-align: right'> </p> </form> </main> </body> </html>"
#define RESPONSE "<!doctype html> <html lang='en'> <head> <meta charset='utf-8'> <meta name='viewport' content='width=device-width, initial-scale=1'> <title>RU Setup</title> <style> *,::after,::before { box-sizing: border-box; } body { margin: 0; font-family: 'Segoe UI', Roboto, 'Helvetica Neue', Arial, 'Noto Sans', 'Liberation Sans'; font-size: 1rem; font-weight: 400; line-height: 1.5; color: #212529; background-color: #f5f5f5; } .form-control { display: block; width: 100%; height: calc(1.5em + .75rem + 2px); border: 1px solid #ced4da; } button { border: 1px solid transparent; color: #fff; background-color: #007bff; border-color: #007bff; padding: .5rem 1rem; font-size: 1.25rem; line-height: 1.5; border-radius: .3rem; width: 100%; } .form-signin { width: 100%; max-width: 400px; padding: 15px; margin: auto; } h1, p { text-align: center; } </style> </head> <body> <main class='form-signin'> <h1>RU Setup</h1> <br/> <p>Settings have been saved successfully...!<br />Restarting the device.</p> </main> </body> </html>"

struct RU_settings {
  char ssid[20];
  char password[20];
  char ru_tag[20];
  char ping_topic[20];
  char drop_sensor_ideal_status[1];
  char mag_spring_conf[8];
} RU_Config = {};

inline void ReadSetting(void) {
  EEPROM.begin(sizeof(struct RU_settings));
  EEPROM.get(0, RU_Config);
}

inline void StartAP(void) {
  Serial.println("Disconnecting current wifi connection");
  WiFi.disconnect();
  delay(10);
  WiFi.softAP("JyotiAP", "");
  Serial.println("Initializing softap");
  server.on("/", handlePortal);
  server.begin();
  while (server.method() != HTTP_POST) {
    server.handleClient();
  }
}

void setup() {
  Serial.begin(115200);  // Initialising if(DEBUG)Serial Monitor
  Serial.println("Reading settings from EEPROM");
  ReadSetting();

  Serial.println(RU_Config.ssid);
  Serial.println(RU_Config.password);
  Serial.println(RU_Config.ru_tag);
  Serial.println(RU_Config.ping_topic);
  Serial.println(RU_Config.drop_sensor_ideal_status[0]);
  Serial.println(RU_Config.mag_spring_conf[0]);
  Serial.println(RU_Config.mag_spring_conf[1]);
  Serial.println(RU_Config.mag_spring_conf[2]);
  Serial.println(RU_Config.mag_spring_conf[3]);
  Serial.println(RU_Config.mag_spring_conf[4]);
  Serial.println(RU_Config.mag_spring_conf[5]);
  Serial.println(RU_Config.mag_spring_conf[6]);
  Serial.println(RU_Config.mag_spring_conf[7]);

  StartAP();
}

void loop() {
  // put your main code here, to run repeatedly:
  // server.handleClient();
}

void handlePortal() {
  if (server.method() == HTTP_POST) {
    // Copying the data into settings structure
    strncpy(RU_Config.ssid, server.arg("ssid").c_str(), sizeof(RU_Config.ssid));
    strncpy(RU_Config.password, server.arg("password").c_str(), sizeof(RU_Config.password));
    strncpy(RU_Config.ru_tag, server.arg("ru_tag").c_str(), sizeof(RU_Config.ru_tag));
    strncpy(RU_Config.ping_topic, server.arg("ping_topic").c_str(), sizeof(RU_Config.ping_topic));
    strncpy(RU_Config.drop_sensor_ideal_status, server.arg("status").c_str(), sizeof(RU_Config.drop_sensor_ideal_status));
    strncpy(RU_Config.mag_spring_conf, server.arg("mag").c_str(), sizeof(RU_Config.mag_spring_conf));

    // terminating the strings with NULL
    RU_Config.ssid[server.arg("ssid").length()] = RU_Config.password[server.arg("password").length()] = '\0';
    RU_Config.ru_tag[server.arg("ru_tag").length()] = RU_Config.ping_topic[server.arg("ping_topic").length()] = '\0';
    RU_Config.drop_sensor_ideal_status[server.arg("status").length()] = RU_Config.mag_spring_conf[server.arg("mag").length()] = '\0';

    // Saving copied structure into EEPROM
    EEPROM.put(0, RU_Config);
    EEPROM.commit();

    // Sending web response html file
    server.send(200, "text/html", RESPONSE);
    Serial.println("Settings saved restarting device");
    delay(1000);
    ESP.restart();
  } else {
    // Sending the html from to get user data
    server.send(200, "text/html", FORM);
  }
}