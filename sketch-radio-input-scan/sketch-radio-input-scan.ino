#include <RCSwitch.h>

// If you use File → Examples → RCSwitch → ReceiveDemo_Advanced,
// the receiver output will also show pulseLength information.

RCSwitch mySwitch = RCSwitch();

// Put the 433 MHz receiver into listening mode for fan remote control codes
void setup() {
  Serial.begin(9600);

  // The receiver is connected to pin D2
  // Interrupts allow the microcontroller, the "brain" of the Arduino,
  // to react immediately to important events without constantly polling the pin.
  // Inside the microcontroller, dedicated hardware continuously monitors the voltage on this pin.
  // Interrupt 0 (INT0) is hardware-mapped to pin D2.
  // Interrupt 1 (INT1) is hardware-mapped to pin D3.
  mySwitch.enableReceive(0);

  Serial.println("Ready to receive...");
}

void loop() {
  if (mySwitch.available()) {
    // Read the received value, bit length, pulse length, and protocol
    unsigned long value = mySwitch.getReceivedValue();
    unsigned int bitLength = mySwitch.getReceivedBitlength();
    unsigned int pulseLength = mySwitch.getReceivedDelay();
    unsigned int protocol = mySwitch.getReceivedProtocol();

    if (value != 0) {
      Serial.print("Received ");
      Serial.print(value);
      Serial.print(" / ");
      Serial.print(bitLength);
      Serial.print(" bit. Pulse length: ");
      Serial.print(pulseLength);
      Serial.print(" us. Protocol: ");
      Serial.println(protocol);
    } else {
      // Signal was received but could not be decoded
      Serial.println("Unknown encoding");
    }

    mySwitch.resetAvailable();
  }
}
