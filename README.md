# ESP32 Local Teachable Machine

This project transforms a standard ESP32 Dev Module into a local web server capable of running a real-time Computer Vision/Machine Learning model (trained with Google Teachable Machine). 

Instead of overwhelming the ESP32's limited memory, this architecture cleverly uses the client's device (Smartphone/PC) to process the neural network via the web browser, sending the predictions back to the ESP32 to display on a 1602 I2C LCD.

## Features:
* **100% Local Web Server:** The HTML/CSS/JS interface is served directly from the ESP32's memory.
* **Smart Offloading:** The heavy AI weights (`.bin` and `.json`) are fetched dynamically from this GitHub repository, saving SPIFFS space.
* **Modern Dark UI:** Beautiful, responsive web interface tailored for mobile and desktop screens.
* **I2C LCD Integration:** Displays the identified object in real-time. Includes automatic character filtering for Spanish accents (tildes) and "ñ" to prevent LCD rendering errors.

## Objects in this machine:
* Ratón (Mouse)
* Botella (Bottle)
* Estuche (Case)
* Mochila (Backpack)
* Típex (Correction tape)
* Calculadora científica (Scientific calculator)

## Hardware Requirements:
* **ESP32 Dev Module** (Standard version, no camera needed).
* **1602 LCD Display** with an **I2C Module**.
* Jumper wires.

### Wiring Guide:
| ESP32 Pin | LCD I2C Pin |
| :---: | :---: |
| 5V / VIN | VCC |
| GND | GND |
| GPIO 21 | SDA |
| GPIO 22 | SCL |

## How to use:
1. **Train your model:** Create your image classification model using [Google Teachable Machine](https://teachablemachine.withgoogle.com/) and download the `.zip` files.
2. **Upload weights to GitHub:** Upload the `model.json`, `metadata.json`, and `weights.bin` to the root of your repository.
3. **Flash the ESP32:** * Open the `.ino` file in the Arduino IDE.
   * Install the `LiquidCrystal_I2C` library.
   * Change the `ssid` and `password` variables to match your home WiFi.
   * Upload the code to your ESP32.
4. **Run the Scanner:**
   * Look at the LCD screen to find your ESP32's local IP address (e.g., `192.168.1.50`).
   * Open a web browser on your smartphone or PC and navigate to that IP.
   * Tap **"Activar Cámara"**, allow camera permissions, and start scanning objects!

> **Note on Mobile Browsers (WebRTC Security):** > Modern mobile browsers restrict camera access on non-HTTPS local networks. To bypass this on Android, navigate to `chrome://flags`, search for "Insecure origins treated as secure", add your ESP32's IP address, enable it, and restart the browser. Alternatively, open the IP address from a PC web browser.

## Dependencies:
* TensorFlow.js
* Teachable Machine Image Library
* Arduino `WiFi.h` & `WebServer.h`
