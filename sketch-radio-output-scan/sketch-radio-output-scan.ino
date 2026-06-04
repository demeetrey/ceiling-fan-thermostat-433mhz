#include <RCSwitch.h>

// pulseLength is the duration of the shortest signal pulse, similar to a "dot" in Morse code.
// 350 us = 1 x pulseLength, usually logical 0.
// 1050 us = 3 x pulseLength, usually logical 1.
// Put the transmitter into scanning mode.
// The goal is to find the correct pulseLength.
// The working pulseLength range was found to be from 190 to 310.
// Alternatively, you can use File → Examples → RCSwitch → ReceiveDemo_Advanced;
// the receiver output will also show pulseLength information.

RCSwitch mySwitch = RCSwitch();

// Array of codes captured from button 1 on the remote control
const unsigned long codes[] = {
  2472780001UL, 2472780016UL, 2472779911UL, 2472779926UL,
  2472779941UL, 2472779956UL, 2472779971UL, 2472779986UL
};
const int numCodes = sizeof(codes) / sizeof(codes[0]);

// pulseLength scan range, from 150 to 650 us with a step of 10
const int startPulse = 150;
const int endPulse = 650;
const int stepPulse = 10;

void setup() {
  Serial.begin(9600);
  mySwitch.enableTransmit(2);
  mySwitch.setRepeatTransmit(15); // Increase repetitions for better reliability
  Serial.println("Start scanning...");
}

void loop() {
  // Iterate through pulseLength values
  for (int pl = startPulse; pl <= endPulse; pl += stepPulse) {
    mySwitch.setPulseLength(pl);

    // Try all codes for each pulseLength value
    for (int i = 0; i < numCodes; i++) {
      Serial.print("Trying code: ");
      Serial.print(codes[i]);
      Serial.print(" with pulseLength: ");
      Serial.println(pl);

      // Send the code multiple times
      for (int r = 0; r < 3; r++) {
        mySwitch.send(codes[i], 32);
        delay(100);
      }
      delay(500); // Pause between different codes
    }
    Serial.println("--- Next pulseLength ---");
  }

  // After the scan is complete, either stop here or start again
  Serial.println("Scan complete. Restarting...");
  delay(10000);
}