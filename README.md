# Curso de Telecomunicaciones para IoT con LoRa y ESP32

En este curso aprenderás:
- Cómo funciona la radio
- Qué es la radio LoRa
- Programar aplicaciones IoT con LoRa y ESP32
- Enviar y recibir paquetes LoRa
- Implementar una conexión puente LoRa + WiFi
- Crear tu propio chat P2P con LoRa

# Pasos de instalación

1.- Clonar el repositorio
```sh
git clone https://github.com/platzi/curso-lora.git
```

2.- Abrir el proyecto en la terminal del ESP-IDF e ir a la carpeta.
(Revisar el curso de programación de microcontroladores con ESP32 para cualquier problema)
```sh
cd curso-lora
```

3.- Configurar la conexión WiFi y los pines de comunicación LoRa
```
idf.py menuconfig
```

4.- Compilar el proyecto
```sh
idf.py build
```

5.- Conectar las dos tarjetas de desarrollo y flashear el binario al puerto correspondiente
```sh
idf.py -p {PUERTO} flash
```

6.- (Opcional) Monitorear cada tarjeta vía USB para ver los logs / prints.
```sh
idf.py -p {PUERTO} monitor
```
* Es posible terminar el modo monitor con `Ctrl + ]`

7.- Una vez conectada la red WiFi, aparecerá la IP en la pantalla OLED de cada tarjeta de desarrollo.
Es necesario que el dispositivo móvil y la tarjeta estén conectadas a la misma red WiFi, se recomienda utilizar el móvil como hotspot.

8.- Abrir la IP en el navegador y comenzar a intercambiar mensajes.

Author: Diana Martinez <diananerdoficial@gmail.com>  
Powered By: Platzi  
