#!/bin/sh

vardir=/var/disks/front_tray

mkdir -p $vardir
#rm -f $vardir/*
ln -fs '/dev/disk/by-path/pci-0000:00:1f.2-scsi-0:0:0:0' $vardir/0
ln -fs '/dev/disk/by-path/pci-0000:03:00.0-scsi-0:0:0:0' $vardir/1
ln -fs '/dev/disk/by-path/pci-0000:03:00.0-scsi-1:0:0:0' $vardir/2
ln -fs '/dev/disk/by-path/pci-0000:04:00.0-scsi-0:0:0:0' $vardir/3
ln -fs '/dev/disk/by-path/pci-0000:04:00.0-scsi-1:0:0:0' $vardir/4
cp -dpf identify-sys.sh identify-model.sh identify-dev.sh identify-size.sh $vardir

