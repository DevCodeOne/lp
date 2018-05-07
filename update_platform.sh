#!/bin/bash

rsync -rl alarm@alarmpi:/opt platform
rsync -rl alarm@alarmpi:/usr platform
