#!/usr/bin/python
# coding=utf8

import socket
import serial
import thread
import re
import time
import eeml
import copy
import pprint
import telnetlib

from httplib import BadStatusLine

import os, sys
lib_path = os.path.abspath('./hausbus2')
sys.path.append(lib_path)

import hausbus2

import config

sensor_values = {}

def pachubeThread():
	# Do stuff to send results to Pachube
	feed = eeml.Pachube(config.API_URL, config.API_KEY, use_https=False)
	while 1 :
		#print sensor_values
		update_data = []
		hausbus2.variables["temperature"].clear
		tn = telnetlib.Telnet(config.TSDB_HOST, config.TSDB_PORT, config.TSDB_TIMEOUT)
		for key, values in sensor_values.items():
			#print sensors[key]["pachube"] + " = " + value
			value = round(float(sum(values)) / len(values),2)
			update_data.append(eeml.Data(config.sensors[key]["pachube"], value, unit=eeml.Celsius()))
			#open(config.log_dir + key,"a").write(str(int(round(time.time()))) + "\t" + str(round(value,2)) + "\n")
			hausbus2.variables["temperature"][config.sensors[key]["hausbus"]] = value
			tsdb_command = "put temperatur "+time.localtime()+" "+value+" sensor="+config.sensors[key]["hausbus"]+"\n"
			tn.write(tsdb_command)
			print tsdb_command
		feed.update(update_data)
		tn.close()
		try:
			feed.put()
		except Exception, err:
			print "Couldn't send data to pachube: ", err
			
		sensor_values.clear()
		time.sleep(30)

def serialThread():
	ser = serial.Serial('/dev/ttyUSB0', 19200)
	temperatur_regex = re.compile("^sensor: ([0-9a-f]{16}) (-?\d+\.\d+)")
	port_regex = re.compile("^windows ([abd]): ([01]{8})")
	while 1:
		line = ser.readline().strip()
		match = temperatur_regex.search(line)
		if (match) :
			if not match.group(1) in sensor_values:
				sensor_values[match.group(1)] = []
			sensor_values[match.group(1)].append(float(match.group(2)))
		match = port_regex.search(line)
		if (match) :
			hausbus2.variables["io_ports"][match.group(1)] = match.group(2)
			updateWindows()

def initWindows():
	hausbus2.variables["windows"]["state"] = copy.deepcopy(config.windows)
	for row_id, row in enumerate(config.windows):
		for col_id, window in enumerate(row):
			if (window != "x" and window != "?") :
				hausbus2.variables["windows"]["state"][row_id][col_id] = "?"

def updateWindows():	
	for row_id, row in enumerate(config.windows):
		for col_id, window in enumerate(row):			
			if (window != "x" and window != "?") :
				if (window[0].lower() in hausbus2.variables["io_ports"]) :
					hausbus2.variables["windows"]["state"][row_id][col_id] = hausbus2.variables["io_ports"][window[0].lower()][int(window[1])]
				else :
					hausbus2.variables["windows"]["state"][row_id][col_id] = "!"

hausbus2.variables["temperature"] = {}
hausbus2.variables["io_ports"] = {}
hausbus2.variables["windows"] = {}
initWindows()

try:
	thread.start_new_thread( pachubeThread, () )
	thread.start_new_thread( serialThread, () )
except:
	print "Error: unable to start thread"


hausbus2.start(80)	
