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

from httplib import BadStatusLine

import os, sys
original_path = os.path.dirname(os.path.realpath(os.path.abspath(sys.argv[0])))
lib_path = original_path + '/hausbus2'
sys.path.append(lib_path)

import hausbus2

import config

sensor_values = {}
running = True

def outputThread():
	temperature = {}
	while running :
		temperature_old = temperature.copy()
		temperature = {}
		for key, values in sensor_values.items():
			value = round(float(sum(values)) / len(values),2)
			temperature[config.sensors[key]["hausbus"]] = value
		if temperature != temperature_old:
			hausbus2.update_group("temperature", temperature, False)
			hausbus2.publish("temperature", retain = True)
		sensor_values.clear()
		time.sleep(30)

def serialThread():
	global io_ports
	ser = serial.Serial('/dev/ttyUSB0', 19200)
	temperatur_regex = re.compile("^sensor: ([0-9a-f]{16}) (-?\d+\.\d+)")
	port_regex = re.compile("^windows ([abd]): ([01]{8})")
	while running:
		line = ser.readline().strip()
		match = temperatur_regex.search(line)
		if (match) :
			if not match.group(1) in sensor_values:
				sensor_values[match.group(1)] = []
			sensor_values[match.group(1)].append(float(match.group(2)))
		match = port_regex.search(line)
		if (match) :
			io_ports_old = io_ports.copy()
			io_ports[match.group(1)] = match.group(2)
			if io_ports_old != io_ports:
				hausbus2.update_group("io_ports", io_ports, False)
				hausbus2.publish("io_ports", retain = True)
				updateWindows()

def initWindows():
	windows["state"] = copy.deepcopy(config.windows)
	for row_id, row in enumerate(config.windows):
		for col_id, window in enumerate(row):
			if (window != "x" and window != "?") :
				windows["state"][row_id][col_id] = "?"

def updateWindows():	
	global windows
	windows_old = windows.copy()
	for row_id, row in enumerate(config.windows):
		for col_id, window in enumerate(row):			
			if (window != "x" and window != "?") :
				if (window[0].lower() in io_ports) :
					windows["state"][row_id][col_id] = io_ports[window[0].lower()][int(window[1])]
				else :
					windows["state"][row_id][col_id] = "!"
	hausbus2.update_group("windows", windows, False)
	hausbus2.publish("windows", retain = True)

hausbus2.start("heizungssteuerung", http_port=80, mqtt_broker = config.mqtt_broker)

io_ports = {}
windows = {}
initWindows()

try:
	thread.start_new_thread( outputThread, () )
	thread.start_new_thread( serialThread, () )
except:
	print >> sys.stderr, "Error: unable to start threads"

try:
	while 1:
		time.sleep(10)
# Ctrl-C interupts our server magic
except KeyboardInterrupt:
	print '^C received, shutting down server'

running = False
hausbus2.clear_retain("temperature")
hausbus2.clear_retain("io_ports")
hausbus2.clear_retain("windows")

hausbus2.stop()
