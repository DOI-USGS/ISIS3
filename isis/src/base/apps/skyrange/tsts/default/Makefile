APPNAME = skyrange

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/m0402852.sky.cub \
	  to=$(OUTPUT)/skyrange.txt > /dev/null;
	$(MV) $(OUTPUT)/skyrange.txt $(OUTPUT)/skyrange.pvl;
	$(APPNAME) from=$(INPUT)/lna3056k.cub \
	  to=$(OUTPUT)/skyrange2.txt > /dev/null;
	$(MV) $(OUTPUT)/skyrange2.txt $(OUTPUT)/skyrange2.pvl;
