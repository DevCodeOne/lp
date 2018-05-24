#!/bin/bash

rsync -rlp alarm@alarmpi:/opt alarm@alarmpi:/usr alarm@alarmpi:/etc/hostapd/ alarm@alarmpi:/etc/systemd/network alarm@alarmpi:/usr/ platform
