#/bin/sh

/sbin/modprobe ich7_gpio
/bin/echo 'setbit GP_IO_SEL2 6 0' > /proc/ich7_gpio
/bin/echo 'setbit GPIO_USE_SEL2 6 1' > /proc/ich7_gpio
/bin/echo 'setbit GP_LVL2 6 0' > /proc/ich7_gpio
/bin/sleep 1
/bin/echo 'setbit GP_LVL2 6 1' > /proc/ich7_gpio

