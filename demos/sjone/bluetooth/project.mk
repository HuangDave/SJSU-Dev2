PLATFORM = lpc17xx
JTAG     = stlink
OPTIMIZE = g

OPENOCD_CONFIG  = $(LIBRARY_DIR)/platforms/targets/lpc17xx/lpc17xx.cfg

purge-debug:
	make purge
	make program
	make debug
