# automata_vezerles - auto remote control
This project is about how an ESP32-webserver (written in Arduino) can be set up without writing the ssid and the password of the router.
(Actually this script was created for hungarian developers so all the tests (Serial.print, html, http requests) and the comments were written by hungarian).

Firstly, when the esp model starts, it looks for a "wifi.json" config file which contains the ssid and the password of the router the esp wants to connect.
If there it is and the ssid and the password are correct then it connects to the net and set up a local dns link.
Otherwise if the congig file contains that there has been already a successful connection, then the setup finishes and the device will try to reconnect periodically in the loop part.

Else the device starts a soft-ap server, which means the user can connect to a setup-wifi and there the user can give the wifi id and password.

Furthermore, there is also a reset-button, if that is pressed more than 3 seconds, then the device will turn to AP-mode immediately in order to set a new ssid.

All these states are visualised with rgb led too.
