#!/bin/sh

monitor=/usr/local/bin/monitor

if [ -x $monitor ]
then
	$monitor --send "$ACTION $DEVNAME"
fi

