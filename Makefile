# enable app support
APP=1
APP_STACKSIZE=300

VPATH += autonomous/
PROJ_OBJ += mr18_driver.o

CRAZYFLIE_BASE=crazyflie-firmware
include $(CRAZYFLIE_BASE)/Makefile