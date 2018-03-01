APPNAME = getsn

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) FROM=$(INPUT)/peaks.cub \
	TO=$(OUTPUT)/peaks.pvl FILE=TRUE OBSERVATION=TRUE \
	APPEND=false;
	$(APPNAME) FROM=$(INPUT)/hirise.cub \
	TO= $(OUTPUT)/hirise.pvl FILE=TRUE OBSERVATION=TRUE \
	APPEND=false;
