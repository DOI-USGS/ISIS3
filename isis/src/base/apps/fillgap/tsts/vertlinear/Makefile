APPNAME = fillgap

include $(ISISROOT)/make/isismake.tsts

commands:
	 $(APPNAME) from=$(INPUT)/peaks.cub \
	  to= $(OUTPUT)/filled.cub \
	 interp=linear \
	 direction=line > /dev/null;
