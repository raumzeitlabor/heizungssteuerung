# RaumTemperaturLaborSensor
Der RaumTemperaturLaborSensor misst die Temperatur im RaumZeitLabor, und postet sie auf Pachube, unter https://pachube.com/feeds/42055

Weitere Infos zum Aufbau gibts im RaumZeitLabor Wiki: http://raumzeitlabor.de/wiki/RaumTemperaturLaborSensor

## Ordner
* <code>firmware</code> beinhaltet die Firmware für den ATmega32 auf dem Sensor-Board
* <code>rtlsd</code> beinhalted den RaumTemperaturLaborSensor Daemon, der die Werte von der seriellen Schnittstelle nimmt und zu Pachube schickt.
