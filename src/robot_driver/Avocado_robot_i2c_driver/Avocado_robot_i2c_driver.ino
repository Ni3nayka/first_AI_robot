/*
#include <HNR-252_DCv0_1.h>
MotorShield motors;

void setup() {
  motors.setup();
  motors.runs(100,100);
  motors.runs(-100,-100);
  motors.runs(22,-22);
  motors.runs(); // stop
}

void loop() {}*/

#include <Wire.h>
#include <HNR-252_DCv0_1.h>

MotorShield motors;

// Аналоговые порты для опроса
const uint8_t analogPins[] = {34, 35, 32, 33, 25, 26, 27}; // 7 аналоговых входов ESP32

#define I2C_ADDRESS_AVOCADO_ROBOT 0x11

// Буфер для I2C коммуникации
#define I2C_BUFFER_SIZE 8
uint8_t i2cBuffer[I2C_BUFFER_SIZE];
volatile uint8_t bufferIndex = 0;

void setup() {
  Serial.begin(115200);
  
  // Инициализация моторов
  motors.setup();
  motors.runs(); // Остановка моторов при старте
  
  // Инициализация I2C slave с адресом 0x11
  Wire.begin(I2C_ADDRESS_AVOCADO_ROBOT);
  Wire.onReceive(receiveEvent);   // обработчик получения данных
  Wire.onRequest(requestEvent);   // обработчик запроса данных
  
  Serial.println("I2C Slave инициализирован с адресом 0x11");
  Serial.println("Готов к приему команд управления моторами и запросам аналоговых портов");
}

void loop() {
  // Основной цикл может быть пустым, так как все обрабатывается в прерываниях
  delay(100);
}

// Обработчик получения данных по I2C
void receiveEvent(int howMany) {
  bufferIndex = 0;
  while (Wire.available() && bufferIndex < I2C_BUFFER_SIZE) {
    i2cBuffer[bufferIndex++] = Wire.read();
  }
  
  // Обработка команд
  if (bufferIndex >= 3) {
    // Команда управления моторами: [команда, скорость_мотора_A, скорость_мотора_B]
    // команда 0x01 - управление моторами
    if (i2cBuffer[0] == 0x01) {
      int8_t speedA = i2cBuffer[1];
      int8_t speedB = i2cBuffer[2];
      
      motors.runs(speedA, speedB);
      Serial.printf("Установка моторов: A=%d%%, B=%d%%\n", speedA, speedB);
    }
  }
}

// Обработчик запроса данных по I2C
void requestEvent() {
  // Команда чтения аналогового порта: [0x02, номер порта (0-6)]
  if (bufferIndex >= 2 && i2cBuffer[0] == 0x02) {
    uint8_t analogPin = i2cBuffer[1];
    if (analogPin < 7) {
      uint16_t value = analogRead(analogPins[analogPin]);
      Wire.write((uint8_t)(value >> 8));   // старший байт
      Wire.write((uint8_t)(value & 0xFF)); // младший байт
      Serial.printf("Чтение аналогового порта %d: %d\n", analogPin, value);
    }
  }
}

/*

Протокол I2C коммуникации:
Управление моторами:

Отправить 3 байта: [0x01, скорость_мотора_A, скорость_мотора_B]

скорость_мотора_A и скорость_мотора_B: от -100 (полный назад) до 100 (полный вперед)

Примеры:

[0x01, 100, 100] - оба мотора вперед на 100%

[0x01, -50, 50] - мотор A назад на 50%, мотор B вперед на 50%

[0x01, 0, 0] - остановка моторов

Чтение аналогового порта:

Отправить 2 байта: [0x02, номер_порта] (0-6)

Затем прочитать 2 байта (старший и младший) - значение АЦП (0-4095 для ESP32)

Особенности реализации:
Используется библиотека HNR-252_DCv0_1.h для управления моторами через метод motors.runs(speedA, speedB)

При старте моторы останавливаются командой motors.runs()

Поддерживается 7 аналоговых входов (по умолчанию для ESP32)

Для работы I2C используются стандартные пины ESP32 (GPIO21 - SDA, GPIO22 - SCL)

Пример использования с мастер-устройством (Arduino или другим контроллером):

cpp
// Мастер отправляет команду на управление моторами
Wire.beginTransmission(0x11);
Wire.write(0x01); // команда управления моторами
Wire.write(75);   // мотор A вперед на 75%
Wire.write(-30);  // мотор B назад на 30%
Wire.endTransmission();

// Мастер запрашивает значение аналогового порта
Wire.beginTransmission(0x11);
Wire.write(0x02); // команда чтения аналогового порта
Wire.write(2);    // порт 2
Wire.endTransmission();

Wire.requestFrom(0x11, 2);
uint16_t analogValue = (Wire.read() << 8) | Wire.read();


*/