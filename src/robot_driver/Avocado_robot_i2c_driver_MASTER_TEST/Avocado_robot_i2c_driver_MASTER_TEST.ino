#include <Wire.h>
#define I2C_ADDRESS_AVOCADO_ROBOT 0x11

int sensor(int number) {
  // Мастер запрашивает значение аналогового порта
  Wire.beginTransmission(I2C_ADDRESS_AVOCADO_ROBOT);
  Wire.write(0x02); // команда чтения аналогового порта
  Wire.write(number);    // порт 2
  Wire.endTransmission();
  Wire.requestFrom(I2C_ADDRESS_AVOCADO_ROBOT, 2);
  return (Wire.read() << 8) | Wire.read();
}

void motor(int left_speed=0, int right_speed=0) {
  Wire.beginTransmission(I2C_ADDRESS_AVOCADO_ROBOT);
  Wire.write(0x01); // команда управления моторами
  Wire.write(constrain(left_speed,-100,100));   // мотор A
  Wire.write(constrain(right_speed,-100,100));  // мотор B
  Wire.endTransmission();
}

void pid() {
  int M = 40;
  long int e = sensor(2) - sensor(3);
  e *= 0.012;
  motor(M+e,M-e);
}

void setup() {
  Serial.begin(9600);
  Wire.begin();
  motor();
}

void loop() {
  // Основной цикл может быть пустым, так как все обрабатывается в прерываниях
  delay(100);
  pid();
  // Serial.print(sensor(2)); Serial.print(" "); Serial.println(sensor(3));
}
