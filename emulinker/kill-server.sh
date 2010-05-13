ps -ef | grep emulinker | grep -v grep | cut -c 9-16 | xargs kill -9
