#!/usr/bin/python
# coding=utf8

import serial
import thread
import re
import time
#import eeml

from hausbus2 import hausbus2

import config

sensor_values = {}

def pachubeThread():
	# Do stuff to send results to Pachube
	feed = eeml.Pachube(config.API_URL, config.API_KEY)
	while 1 :
		time.sleep(30)
		#print sensor_values
		update_data = []
		hausbus2.variables["temperature"].clear
		for key, value in sensor_values.items():
			#print sensors[key]["pachube"] + " = " + value
			update_data.append(eeml.Data(config.sensors[key]["pachube"], value, unit=eeml.Celsius()))
			open(config.log_dir + key,"a").write(str(int(round(time.time()))) + "\t" + value + "\n")
			hausbus2.variables["temperature"][config.sensors[key]["hausbus"]] = value
		feed.update(update_data)
		feed.put()
		sensor_values.clear()
		

def serialThread():
	ser = serial.Serial('/dev/ttyUSB0', 19200)
	regex = re.compile("^sensor: ([0-9a-f]{16}) (-?\d+\.\d+)")
	while 1:
		line = ser.readline().strip()
		match = regex.search(line)
		if (match) :
			sensor_values[match.group(1)] = match.group(2)

hausbus2.variables["temperature"] = {}		
try:
	thread.start_new_thread( pachubeThread, () )
	thread.start_new_thread( serialThread, () )
except:
	print "Error: unable to start thread"


hausbus2.start(8080)	
