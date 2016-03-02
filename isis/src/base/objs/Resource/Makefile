ifeq ($(ISISROOT), $(BLANK))
.SILENT:
error:
	echo "Please set ISISROOT";
else
	include $(ISISROOT)/make/isismake.objs
    GEOSLIB += -lgeos_c
endif
