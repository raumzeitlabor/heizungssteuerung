default:
	g++ -Wall -pedantic -g -o log temperaturlogger.cpp -lserial

deploy:
	scp logger.py heizung:~/RaumTemperaturLaborSensor/software/
	scp hausbus2/hausbus2.py heizung:~/RaumTemperaturLaborSensor/software/hausbus2/

getlog:
	scp -r dockstar:~/logger/logs .

plot:
	gnuplot plot.gpl

pushbb:
	scp temp1.svg temp2.svg bb:/var/www/temperatur/
