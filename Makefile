all: monitor

CFLAGS = -g -Wall 

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

monitor_OBJ = monitor.o string_out.o packet.o raid_monitor.o fifo_interface.o device_db.o util.o serial_interface.o readopt.o options.o

monitor: $(monitor_OBJ)
	$(CC) -o $@ $(monitor_OBJ)

install: monitor
	install monitor /usr/local/bin
	install thecus_block_event.sh /usr/local/bin

clean:
	rm -f monitor 
	rm -f *.o

