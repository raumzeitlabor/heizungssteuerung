# Heizungssteuerung
Die RZL Heizungssteuerung misst die Temperatur im RaumZeitLabor, und postet sie auf Cosm, unter https://cosm.com/feeds/42055

Weitere Infos zum Aufbau gibts im RaumZeitLabor Wiki: http://raumzeitlabor.de/wiki/RaumTemperaturLaborSensor

## Ordner
* <code>firmware</code> beinhaltet die Firmware für den ATmega32 auf dem Sensor-Board
* <code>software</code> beinhaltet den Heizungssteuerungs-Daemon, der die Werte von der seriellen Schnittstelle nimmt und an Cosm und OpenTSDB(via tcollector, siehe unten) sendet. Er stellt zudem die empfangenen Werte via Hausbus2 zu Verfügung.

## tcollector
Die Heizungssteuerung soll mit tcollector (http://opentsdb.net/tcollector.html) laufen. Um Code-Duplikate zu vermeiden, wird dazu ein Symlink in collectors/0/ auf logger.oy gesetzt.
