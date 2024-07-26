
# LoRa and ESP32 Communication Project

This repository contains code and instructions for setting up a communication system using LoRa and ESP32. The project includes examples of LoRa communication with an ESP32 microcontroller, demonstrating basic setup, sending, and receiving messages.

## Table of Contents

- [Introduction](#introduction)
- [Hardware Required](#hardware-required)
- [Libraries Required](#libraries-required)
- [Installation](#installation)
- [Code Explanation](#code-explanation)
  - [Initialization](#initialization)
  - [Sending Data](#sending-data)
  - [Receiving Data](#receiving-data)
- [Usage](#usage)
- [Contributing](#contributing)
- [License](#license)

## Introduction

This project demonstrates how to use the LoRa protocol with the ESP32 microcontroller for wireless communication. The provided examples cover basic LoRa initialization, sending and receiving messages, and using sensors like DHT11 for transmitting environmental data.

## Hardware Required

- ESP32 microcontroller
- LoRa module (e.g., RFM95)
- Jumper wires
- Breadboard
- DHT11 temperature and humidity sensor (optional for specific examples)
- Buzzer (optional)

## Libraries Required

Ensure you have the following libraries installed in your Arduino IDE:

- [RadioHead](http://www.airspayce.com/mikem/arduino/RadioHead/)
- [DHT sensor library](https://github.com/adafruit/DHT-sensor-library) (optional for specific examples)

To install these libraries, go to `Sketch -> Include Library -> Manage Libraries...` and search for the library names.

## Installation

1. Clone this repository to your local machine:

   ```sh
   git clone https://github.com/yourusername/LoRa-ESP32-Communication.git
   cd LoRa-ESP32-Communication
   ```

2. Open the desired example code in the Arduino IDE.

3. Connect your ESP32 and LoRa module as described in the code comments.

4. Upload the code to your ESP32.

## Code Explanation

### Initialization

The initialization part of the code sets up the LoRa module and configures the necessary pins:

```cpp
#include <SPI.h>
#include <RH_RF95.h>

#define RFM95_CS 26
#define RFM95_RST 12
#define RFM95_INT 25

#define RF95_FREQ 433.0

RH_RF95 rf95(RFM95_CS, RFM95_INT);

void setup() {
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  Serial.begin(115200);
  delay(100);

  // Manual Reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  if (!rf95.init()) {
    Serial.println(">> LoRa radio initialization failed.");
    while (1);
  }

  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println(">> Set frequency failed.");
    while (1);
  }

  rf95.setTxPower(18);
  Serial.println(">> LoRa radio initialized successfully.");
}
```

### Sending Data

To send data, the `send` function is used:

```cpp
void loop() {
  delay(1000);
  const char *msg = "Hello World!";
  rf95.send((uint8_t *)msg, strlen(msg));
  rf95.waitPacketSent();
  Serial.println(">> Message sent.");
}
```

### Receiving Data

To receive data, the `recv` function is used:

```cpp
void loop() {
  if (rf95.available()) {
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    if (rf95.recv(buf, &len)) {
      Serial.print(">> Received: ");
      Serial.println((char*)buf);
    }
  }
}
```

## Usage

1. Open the example code you want to use in the Arduino IDE.
2. Connect the ESP32 and LoRa module according to the pin definitions in the code.
3. Upload the code to your ESP32.
4. Open the Serial Monitor to view the communication logs.

## Contributing

Contributions are welcome! Please open an issue or submit a pull request with your improvements.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

<a href="https://nowpayments.io/donation?api_key=REWCYVC-A1AMFK3-QNRS663-PKJSBD2&source=lk_donation&medium=referral" target="_blank">
     <img src="https://nowpayments.io/images/embeds/donation-button-black.svg" alt="Crypto donation button by NOWPayments">
    </a>
