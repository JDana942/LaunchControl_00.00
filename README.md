# LaunchControl_V2

Reason for upgrade:
Wifi based rocket ignition and thrust measurement using an ESP32 and HX711 Strain gauge.
Making V1 I ran into some issues trying to accuratly measure thrust using a Pi W due to noise and system interupts. Also, seemed that using the main UART for the BLE bus left me with a lower Quality mini UART to be assigned to read off the ADC converter causing performance issues. instead of trying yet another work around I decided to move to the ESP32 platform hosting as the server and integrate a safer ignition system.
