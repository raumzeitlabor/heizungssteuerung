#!/bin/bash
cd /root/heizungssteuerung/logger
screen -dmS logger  ./log /dev/ttyUSB0 19200

