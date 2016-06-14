TOPDIR ?= ../../..
include $(TOPDIR)/.config

$(RAINMASTER_SUPPORT)_SRC += services/intewa/rainmaster/rainmaster.c
$(RAINMASTER_SUPPORT)_SRC += services/intewa/rainmaster/rainmaster_menu.c
##$(RAINMASTER_SUPPORT)_SRC += services/intewa/rainmaster/rainmaster_core.c
##$(RAINMASTER_SUPPORT)_SRC += services/intewa/rainmaster/rainmaster_sensors.c
##$(RAINMASTER_SUPPORT)_SRC += services/intewa/rainmaster/rainmaster_net.c
##$(RAINMASTER_SUPPORT)_SRC += services/intewa/rainmaster/aqualoop_gui.c

##$(RAINMASTER_SUPPORT)_ECMD_SRC += services/intewa/rainmaster/rainmaster_ecmd.c

##############################################################################
# generic fluff
include $(TOPDIR)/scripts/rules.mk


DOXYGEN := doxygen

doc: $(HEADERS) $(SOURCES) Doxyfile
	$(DOXYGEN) Doxyfile

	