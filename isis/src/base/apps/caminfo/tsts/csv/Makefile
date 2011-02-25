APPNAME = caminfo

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) FROM=$(INPUT)/input.cub \
	TO=$(OUTPUT)/truth.txt FORMAT=csv APPEND=false \
	STATISTICS=true CAMSTATS=true GEOMETRY=true spice=true > /dev/null;
