#!/bin/bash
stty -F /dev/ttyUSB0 cs8 9600 ignbrk -brkint -icrnl -imaxbel -opost -onlcr -isig -icanon -iexten -echo -echoe -echok -echoctl -echoke noflsh -ixon -crtscts 

while [ 1 ]
do
head -n 1 /dev/ttyUSB0 | xargs -I xxx sed 's/%VALUE%/xxx/g' template.json | curl --request PUT -d @- --header "X-PachubeApiKey: YOUR_PACHUBE_API_KEY" http://api.pachube.com/v2/feeds/42055
done
