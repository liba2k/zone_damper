#include <Arduino.h>
#include "web_assets.h"
#include <WebSocketsServer.h>
#include "ESP_IOT.h"

#ifdef ARDUINO_ARCH_ESP32
  #define A 35
  #define B 37
#endif

#define LED 15

#define SECOND_IN_MS 1000
#define RESET_POS_TIME (10 * SECOND_IN_MS)
#define PRE_MOVMENT_TIME (2.5 * SECOND_IN_MS)
#define DEG_TIME (8.5 * SECOND_IN_MS / 90)

enum MovingState {
    not_moving = 0,
    moving_to_reset,
    moving_to_angle
} moving_state =MovingState::not_moving;

ulong dest_millis = 0;
#defing IMPASSIBLE_ANGLE 99 
uint dest_angle = IMPASSIBLE_ANGLE;

void handle_index() {
    IOT.server.sendHeader("Content-Encoding", "gzip");
    IOT.server.send_P(200, "text/html", index_html, sizeof(index_html));
}
void handle_favicon() {
    IOT.server.sendHeader("Content-Encoding", "gzip");
    IOT.server.send_P(200, "image/x-icon", damper_png, sizeof(damper_png));
}
void handle_reconnecting_websocket() {
    IOT.server.sendHeader("Content-Encoding", "gzip");
    IOT.server.send_P(200, "text/javascript	", reconnecting_websocket_js, sizeof(reconnecting_websocket_js));
}

void handle_set_GPIO() {
    digitalWrite(A, LOW);
    digitalWrite(B, LOW);
    String val(IOT.server.arg("val"));
    if (val == "A")
    {
        IOT.server.send(200, "text/plain", "A");
        digitalWrite(A, HIGH);
    } else if (val == "B") {
        IOT.server.send(200, "text/plain", "B");
        digitalWrite(B, HIGH);
    } else {
        IOT.server.send(200, "text/plain", "off");
    }
}

void handle_get_angle() {
    IOT.server.send(200, "text/plain", String(dest_angle));
}

void handle_set_angle() {
    digitalWrite(A, LOW);
    digitalWrite(B, LOW);
    String val(IOT.server.arg("val"));
    uint angle = val.toInt();
    if (angle < 0 or angle > 90) {
        IOT.server.send(400, "text/plain", "Angle not supported");
        return;
    }
    if (angle == dest_angle) {
        IOT.server.send(200, "text/plain", "Already there");
        return;
    }
    if (dest_angle == 90) {
        dest_angle = angle;
        moving_state = MovingState::moving_to_angle;
        digitalWrite(B, HIGH);
        dest_millis = millis() + (90-dest_angle) * DEG_TIME;
        IOT.server.send(200, "text/plain", "running in reverse");
    } else {
        dest_angle = angle;
        moving_state = MovingState::moving_to_reset;
        digitalWrite(B, HIGH);
        dest_millis = millis() + RESET_POS_TIME;
        IOT.server.send(200, "text/plain", "Moving");
    }

}

void setup() {
    Serial.begin(115200);
    digitalWrite(A, LOW);
    digitalWrite(B, LOW);
    pinMode(A, OUTPUT);
    pinMode(B, OUTPUT);
    pinMode(LED, OUTPUT);

    IOT.initIOT("ThisPa55word!","zone_damper");
    IOT.server.on("/", handle_index);
    IOT.server.on("/damper.png", handle_favicon);
    IOT.server.on("/reconnecting_websocket.js", handle_reconnecting_websocket);
    IOT.server.on("/set_gpio", handle_set_GPIO);
    IOT.server.on("/set_angle", handle_set_angle);
    IOT.server.on("/get_angle", handle_get_angle);
}

void loop() 
{
    IOT.handle();
    ulong now = millis();
    if (moving_state == MovingState::moving_to_reset && now >= dest_millis) {
        moving_state = MovingState::moving_to_angle;
        digitalWrite(B, LOW);
        if (dest_angle == 0)
            return;
        dest_millis = now + PRE_MOVMENT_TIME + dest_angle * DEG_TIME;
        digitalWrite(A, HIGH);
    } else if (moving_state == MovingState::moving_to_angle && now >= dest_millis) {
        digitalWrite(A, LOW);
        digitalWrite(B, LOW);
    }
}
