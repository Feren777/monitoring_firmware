TOPDIR ?= ../../..
include $(TOPDIR)/.config

$(AQUALOOP_SUPPORT)_SRC += services/intewa/aqualoop/aqualoop.c
$(AQUALOOP_SUPPORT)_SRC += services/intewa/aqualoop/aqualoop_core.c
$(AQUALOOP_SUPPORT)_SRC += services/intewa/aqualoop/aqualoop_menu.c
$(AQUALOOP_SUPPORT)_SRC += services/intewa/aqualoop/aqualoop_net.c
$(AQUALOOP_SUPPORT)_SRC += services/intewa/aqualoop/aqualoop_sensors.c
##$(AQUALOOP_SUPPORT)_SRC += services/intewa/aqualoop/aqualoop_gui.c

$(AQUALOOP_SUPPORT)_ECMD_SRC += services/intewa/aqualoop/aqualoop_ecmd.c

##############################################################################
# generic fluff
include $(TOPDIR)/scripts/rules.mk


DOXYGEN := doxygen

doc: $(HEADERS) $(SOURCES) Doxyfile
	$(DOXYGEN) Doxyfile

	