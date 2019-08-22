# ACController
This is a open source project with ESP8266 that aims to control any air conditioner type over IR protocol using Android/iOS application or voice assistant like Google Home / Alexa.

# PCB Preview (First prototype PCB)
![image](https://i.ibb.co/BCLy09t/Screenshot-2019-08-21-at-23-34-39.png)

Information for schematic files and PCB can be found here: https://easyeda.com/dimitarivanovit/irblaster

# How does it work ?
  * When the device is powered on a webserver is started to setup WiFi connection.
  * After that SSDP service is started.
  * Mobile application find the device over SSDP protocol
  * From the application you first have to init the device before using it with voice assitant in order for that to work mobile device and hardware unit should be at the same wifi network.
  * After this process you can control your AC threw mobile application UI (you have to press a button to send a command) or the voice assitant (each voice action is command to AC)
 
# Mobile applicaiton source code
  * https://github.com/dimityrivanov/Android-ACController
  
# Roadmap
  * Develop iOS application like the Android one
  * Find testers to evalute that IRSend codes for the AC Brands currentlly implemented are working
  * Add more ACUnit support
  * Modify code endpoint to return list of supported devices instead of hardcoding them into the mobile application
  * Add support for alexa voice commands
  * Clean code
  
  
# Validated Brands
 * Mitsubishi IR Code confirmed by @dimityrivanov
 * Panosonic IR Code not confirmed
 * Daikin IR Code not confirmed
  
