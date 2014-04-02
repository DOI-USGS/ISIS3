APPNAME = lorri2isis

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/lor_0034974380_0x630_sci_1.fit \
	  to=$(OUTPUT)/lor_0034974380_0x630_sci_1.cub \
	  > /dev/null;
	catlab from=$(OUTPUT)/lor_0034974380_0x630_sci_1.cub \
	  to=$(OUTPUT)/lor_0034974380_0x630_sci_1.pvl> /dev/null;


