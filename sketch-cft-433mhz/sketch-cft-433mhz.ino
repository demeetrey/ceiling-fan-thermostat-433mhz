  #include <OneWire.h>
  #include <RCSwitch.h>
  #include <DallasTemperature.h>

  #define FIRMWARE_VERSION "1.1.1"

  // Time conversion helpers
  #define S(x) ((x) * 1000UL)    // seconds to milliseconds
  #define M(x) ((x) * 60000UL)   // minutes to milliseconds
  #define H(x) ((x) * 3600000UL) // hours to milliseconds

  // RF transmitter
  RCSwitch mySwitch = RCSwitch();

  template <size_t N>
  class LoopQueue {
    private:
      uint8_t index = 0;
      uint32_t list[N];

    public:
      LoopQueue(const uint32_t (&arr)[N]) {
        memcpy(list, arr, sizeof(list));
      }

      uint32_t generateNext() {
        uint32_t value = list[index];

        index++;
        if (index >= N) {
          index = 0;
        }

        return value;
      }
  };

  // Status LEDs
  const int led_G = 4;
  const int led_R = 6;

  // RF command sequences
  // Codes in each array are sorted in ascending order.
  // The second and third arrays are shifted relative to the first one
  // by one and two steps respectively,
  // to match the receiver's cyclic sequence.
  LoopQueue<8> button_2_codes = LoopQueue<8>({2472780421UL, 2472780436UL, 2472780455UL, 2472780470UL, 2472780481UL, 2472780496UL, 2472780515UL, 2472780530UL});
  LoopQueue<8> button_toggle_codes = LoopQueue<8>({2472780183UL, 2472780196UL, 2472780213UL, 2472780226UL, 2472780243UL, 2472780256UL, 2472780273UL, 2472780166UL});
  LoopQueue<8> button_stop_codes = LoopQueue<8>({2472781475UL, 2472781490UL, 2472781509UL, 2472781524UL, 2472781543UL, 2472781558UL, 2472781441UL, 2472781456UL});

  // Current fan state tracked by the controller
  bool is_fan_started = false;

  // Temperature thresholds, °C
  const short int MIN_TEMPERATURE = 37;
  const short int MAX_TEMPERATURE = 40;

  // DS18B20 sensor pins
  const int DS18B20_1 = 8;
  const int DS18B20_2 = 10;

  OneWire oneWire1(DS18B20_1);
  OneWire oneWire2(DS18B20_2);

  DallasTemperature sensor1(&oneWire1);
  DallasTemperature sensor2(&oneWire2);

  void setup() {
    Serial.begin(9600);

    // RF transmitter settings
    mySwitch.enableTransmit(2);
    mySwitch.setProtocol(1);
    mySwitch.setPulseLength(270);

    pinMode(led_G, OUTPUT);
    pinMode(led_R, OUTPUT);

    startSystemLED();

    sensor1.begin();
    sensor2.begin();
    
    // Sensor 1 missing: blink once per second, 10 times
    if (sensor1.getDeviceCount() == 0) {
      for (int i = 10; i > 0; i--) {
        digitalWrite(led_R, HIGH);
        delay(500);
        digitalWrite(led_R, LOW);
        delay(500);
      }
    }

    // Sensor 2 missing: blink twice per second, 10 times
    if (sensor2.getDeviceCount() == 0) {
      for (int i = 10; i > 0; i--) {
        digitalWrite(led_R, HIGH);
        delay(200);
        digitalWrite(led_R, LOW);
        delay(200);
        digitalWrite(led_R, HIGH);
        delay(200);
        digitalWrite(led_R, LOW);
        delay(400);
      }
    }

    // Startup fan calibration: turn on, switch direction, then turn off
    delay(S(1));
    fanOn();
    delay(S(3));
    toggleFanDirection();
    delay(S(2));
    fanOff();
    delay(S(10));
  }

  // Startup indication
  void startSystemLED() {
    for (int i = 3; i > 0; i--) {
      digitalWrite(led_G, HIGH);
      delay(500);
      digitalWrite(led_G, LOW);
      delay(500);
    }

    digitalWrite(led_G, HIGH);
  }

  void fanOn() {
    digitalWrite(led_R, HIGH);
    mySwitch.setRepeatTransmit(30);
    mySwitch.send(button_2_codes.generateNext(), 32);
    is_fan_started = true;
  }

  void toggleFanDirection() {
    // Repeat the signal 30 times, which is slightly more than 3 seconds
    // About 3 seconds are required to switch between winter and summer mode
    digitalWrite(led_R, LOW);
    delay(500);
    digitalWrite(led_R, HIGH);
    mySwitch.setRepeatTransmit(30);
    mySwitch.send(button_toggle_codes.generateNext(), 32);
  }

  void fanOff() {
    digitalWrite(led_R, LOW);
    delay(500);
    mySwitch.setRepeatTransmit(30);
    mySwitch.send(button_stop_codes.generateNext(), 32);
    digitalWrite(led_R, HIGH);
    delay(200);
    digitalWrite(led_R, LOW);
    is_fan_started = false;
  }

  void loop() {
    // Request temperature from both sensors
    // Sends a command to all DS18B20 sensors on this bus to start temperature conversion
    // Conversion takes time, from 94 ms to 750 ms depending on the selected resolution
    sensor1.requestTemperatures();
    sensor2.requestTemperatures();
    
    delay(1000); // Wait for DS18B20 temperature conversion to complete
    
    // Read the measured temperature from a specific sensor on the bus
    float temp1 = sensor1.getTempCByIndex(0);
    float temp2 = sensor2.getTempCByIndex(0);

    // Deadlock detector
    // if the fan is expected to be running but the temperature is still too high,
    // reset the fan state with OFF -> ON to keep the RF code sequence in sync.
    if (is_fan_started && (temp1 > MAX_TEMPERATURE || temp2 > MAX_TEMPERATURE)) {
      // Alarm system
      for (int i = 3; i > 0; i--) {
        digitalWrite(led_G, LOW);
        digitalWrite(led_R, HIGH);
        delay(200);
        digitalWrite(led_G, HIGH);
        digitalWrite(led_R, LOW);
        delay(200);
      }

      fanOff();
      delay(S(3));
      fanOn();
      delay(M(5));
      return;
    }

    if (is_fan_started) {
      if (temp1 < MIN_TEMPERATURE || temp2 < MIN_TEMPERATURE) {
        fanOff();
      }
    } else {
      if (temp1 > MAX_TEMPERATURE || temp2 > MAX_TEMPERATURE) {
        fanOn();
        delay(M(4));
      }
    }

    delay(M(1));
  }