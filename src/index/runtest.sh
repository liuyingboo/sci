#!/bin/bash
count=837
homedir="/home/lyb"
rm -rf ~/fitsindex/
./onlineIndex -n ${count} -d ${homedir}
./offlineIndex -d ${homedir}