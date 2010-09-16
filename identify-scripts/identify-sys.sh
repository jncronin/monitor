#!/bin/sh

echo `/var/disks/front_tray/identify-dev.sh $1 | /bin/sed 's:/dev:/sys/block:'`/device | /bin/grep sd

