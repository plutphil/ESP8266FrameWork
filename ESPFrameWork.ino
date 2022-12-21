#include"ESPFrameWork.hpp"
#include"LEDMatrix.hpp"
void setup() {
  Serial.begin(115200);
  wifi_setup();
  ota_setup();
}

void loop() {
  framework_loop();
}
