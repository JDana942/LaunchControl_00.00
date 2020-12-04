# LaunchControl_V2
Wifi based rocket ignition and thrust measurement with ESP32 and HX711.

Reason for upgrade:
Making V1 I ran into some issues trying to accuratly measure thrust using a Pi W due to noise and system interupts. Also, seemed that using the main UART for the BLE bus left me with a lower Quality mini UART to be assigned to read off the ADC converter causing performance issues. instead of trying yet another work around I decided to move to the ESP32 platform hosting as the server and integrate a safer ignition system.

In Progress:
Output data to graph displayed on webpage for quick analysis
add button for 1kg calibration
Output to SD for further refinement and Documentation (Like i'll ever do that)
Clean up the launch proccess and ability to fully disarm rocket in case of a missfire.
Remove debug code
