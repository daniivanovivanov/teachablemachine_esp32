#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ========================================================
// 1. Wi-Fi Settings
// ========================================================
const char* ssid = "TU_NOMBRE_DE_WIFI";
const char* password = "TU_CONTRASEÑA_DE_WIFI";

WebServer server(80);
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ========================================================
// 2. Full Website
// ========================================================
const char paginaWeb[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="es">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Machine Learning - ESP32</title>
    
    <script src="https://cdn.jsdelivr.net/npm/@tensorflow/tfjs@latest/dist/tf.min.js"></script>
    <script src="https://cdn.jsdelivr.net/npm/@teachablemachine/image@latest/dist/teachablemachine-image.min.js"></script>
    <link href="https://fonts.googleapis.com/css2?family=Orbitron:wght@700&display=swap" rel="stylesheet">
    
    <style>
        * { box-sizing: border-box; margin: 0; padding: 0; }
        body { 
            font-family: '-apple-system', BlinkMacSystemFont, 'Segoe UI', Roboto, Helvetica, Arial, sans-serif;
            background: linear-gradient(135deg, #0f0c20 0%, #06060c 100%);
            color: #ffffff;
            min-height: 100vh;
            display: flex;
            flex-direction: column;
            align-items: center;
            justify-content: center;
            padding: 20px;
        }
        .card {
            background: rgba(255, 255, 255, 0.03);
            backdrop-filter: blur(16px);
            -webkit-backdrop-filter: blur(16px);
            border: 1px solid rgba(255, 255, 255, 0.07);
            padding: 30px 20px;
            border-radius: 24px;
            width: 100%;
            max-width: 440px;
            text-align: center;
            box-shadow: 0 20px 40px rgba(0,0,0,0.7);
        }
        h1 {
            font-family: 'Orbitron', sans-serif;
            font-size: 2.2rem;
            margin-bottom: 10px;
            color: #00f0ff;
            text-shadow: 0 0 15px rgba(0, 240, 255, 0.5);
            letter-spacing: 1px;
        }
        p.subtitle {
            font-size: 1rem;
            color: #8a8aa3;
            margin-bottom: 25px;
        }
        .btn-container { margin-bottom: 20px; }
        button {
            background: linear-gradient(90deg, #7f00ff 0%, #00f0ff 100%);
            color: white;
            border: none;
            padding: 16px 32px;
            font-size: 1.1rem;
            font-weight: 600;
            border-radius: 50px;
            cursor: pointer;
            width: 100%;
            transition: all 0.3s ease;
            box-shadow: 0 8px 20px rgba(0, 240, 255, 0.3);
        }
        button:hover {
            transform: translateY(-2px);
            box-shadow: 0 12px 26px rgba(0, 240, 255, 0.5);
        }
        
        #webcam-container {
            display: flex;
            justify-content: center;
            margin-top: 15px;
        }
        canvas {
            border-radius: 16px;
            border: 2px solid rgba(255, 255, 255, 0.1);
            max-width: 100%;
            height: auto;
            box-shadow: 0 10px 25px rgba(0,0,0,0.5);
            background-color: #000;
        }
        #result-badge {
            margin-top: 25px;
            padding: 15px;
            background: rgba(0, 240, 255, 0.07);
            border: 1px solid rgba(0, 240, 255, 0.2);
            border-radius: 14px;
            font-size: 1.3rem;
            font-weight: 700;
            color: #00f0ff;
            text-transform: uppercase;
            letter-spacing: 1px;
            min-height: 58px;
            display: flex;
            align-items: center;
            justify-content: center;
        }
        .footer-status {
            font-size: 0.8rem;
            color: #525266;
            margin-top: 20px;
        }
    </style>
</head>
<body>

    <div class="card">
        <h1>Machine Learning</h1>
        <p class="subtitle">Servido localmente en la ESP-32</p>
        
        <div class="btn-container">
            <button type="button" id="btn-start" onclick="init()">Activar Cámara</button>
        </div>
        
        <div id="webcam-container"></div>
        <div id="result-badge">Esperando cámara...</div>
        
        <div id="status" class="footer-status">Listo para conectar</div>
    </div>

    <script type="text/javascript">
        const modelURL = "https://raw.githubusercontent.com/daniivanovivanov/teachablemachine_esp32/refs/heads/main/model.json";
        const metadataURL = "https://raw.githubusercontent.com/daniivanovivanov/teachablemachine_esp32/refs/heads/main/metadata.json";

        let model, webcam, maxPredictions;
        let ultimaVezEnviado = 0;

        async function init() {
            document.getElementById("btn-start").style.display = "none";
            document.getElementById("result-badge").innerHTML = "Cargando Red Neuronal...";
            document.getElementById("status").innerHTML = "Descargando modelo desde GitHub...";

            try {
                model = await tmImage.load(modelURL, metadataURL);
                maxPredictions = model.getTotalClasses();

                document.getElementById("status").innerHTML = "Solicitando permiso de cámara...";
                
                const flip = false; 
                webcam = new tmImage.Webcam(360, 360, flip); 
                
                await webcam.setup({ facingMode: "environment" }); 
                await webcam.play();
                
                document.getElementById("webcam-container").appendChild(webcam.canvas);
                document.getElementById("result-badge").innerHTML = "Buscando objetos...";
                document.getElementById("status").innerHTML = "Sistema en línea";
                
                window.requestAnimationFrame(loop);
            } catch (error) {
                document.getElementById("result-badge").innerHTML = "CÁMARA BLOQUEADA";
                document.getElementById("status").innerHTML = "Por seguridad, tu navegador bloquea la cámara en redes locales sin HTTPS.";
                alert("¡No se pudo iniciar la cámara!\n\nSi estás en móvil, tu navegador bloquea la cámara porque es una conexión HTTP local.\nDebes aplicar el truco de chrome://flags o acceder desde el navegador de tu PC.");
                console.error(error);
                document.getElementById("btn-start").style.display = "block";
            }
        }

        async function loop() {
            webcam.update(); 
            await predict();
            window.requestAnimationFrame(loop);
        }

        async function predict() {
            const prediction = await model.predict(webcam.canvas);
            
            let mejorClase = "Ninguno";
            let mayorProbabilidad = 0;

            for (let i = 0; i < maxPredictions; i++) {
                if(prediction[i].probability > mayorProbabilidad) {
                     mayorProbabilidad = prediction[i].probability;
                     mejorClase = prediction[i].className;
                }
            }

            if (mayorProbabilidad > 0.70) {
                document.getElementById("result-badge").innerHTML = mejorClase + " (" + (mayorProbabilidad * 100).toFixed(0) + "%)";
            } else {
                document.getElementById("result-badge").innerHTML = "Escaneando...";
                mejorClase = "Buscando...";
            }

            let ahora = Date.now();
            if(mayorProbabilidad > 0.85 && (ahora - ultimaVezEnviado > 2000)) {
                 ultimaVezEnviado = ahora;
                 document.getElementById("status").innerHTML = "Enviando '" + mejorClase + "' al LCD...";
                 
                 fetch(`/prediccion?objeto=${encodeURIComponent(mejorClase)}`)
                    .then(response => {
                        document.getElementById("status").innerHTML = "Sincronizado con LCD ✅";
                    })
                    .catch(error => {
                        document.getElementById("status").innerHTML = "Error de enlace por WiFi ❌";
                    });
            }
        }
    </script>
</body>
</html>
)=====";

// ========================================================
// 3. ESP-32 Code
// ========================================================
void setup() {
  Serial.begin(115200);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Iniciando WiFi...");

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("IP del ESP32:");
  lcd.setCursor(0, 1);
  lcd.print(WiFi.localIP());

  Serial.println("\nWiFi Conectado!");
  Serial.print("Entra en esta IP: http://");
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, []() {
    server.send(200, "text/html", paginaWeb);
  });

  server.on("/prediccion", HTTP_GET, []() {
    if (server.hasArg("objeto")) {
      String objetoDetectado = server.arg("objeto");
      
      objetoDetectado.replace("á", "a");
      objetoDetectado.replace("é", "e");
      objetoDetectado.replace("í", "i");
      objetoDetectado.replace("ó", "o");
      objetoDetectado.replace("ú", "u");
      
      objetoDetectado.replace("Á", "A");
      objetoDetectado.replace("É", "E");
      objetoDetectado.replace("Í", "I");
      objetoDetectado.replace("Ó", "O");
      objetoDetectado.replace("Ú", "U");
      
      objetoDetectado.replace("ñ", "n");
      objetoDetectado.replace("Ñ", "N");
      
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Detectado:");
      lcd.setCursor(0, 1);
      lcd.print(objetoDetectado);

      server.send(200, "text/plain", "LCD Actualizado");
    } else {
      server.send(400, "text/plain", "Error de peticion");
    }
  });

  server.begin();
}

void loop() {
  server.handleClient();
}