APPNAME = lorri2isis

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/lor_0034974377_0x630_sci_1.fit \
	  to=$(OUTPUT)/lor_0034974377_0x630_sci_1_Truth.cub \
	  error=$(OUTPUT)/lor_0034974377_0x630_sci_1_Error_Truth.cub \
	  quality=$(OUTPUT)/lor_0034974377_0x630_sci_1_Quality_Truth.cub \
	  > /dev/null;
	catlab from=$(OUTPUT)/lor_0034974377_0x630_sci_1_Truth.cub \
	  to=$(OUTPUT)/lor_0034974377_0x630_sci_1_Truth.pvl> /dev/null;
	catlab from=$(OUTPUT)/lor_0034974377_0x630_sci_1_Error_Truth.cub \
	  to=$(OUTPUT)/lor_0034974377_0x630_sci_1_Error_Truth.pvl> /dev/null;
	catlab from=$(OUTPUT)/lor_0034974377_0x630_sci_1_Quality_Truth.cub \
	  to=$(OUTPUT)/lor_0034974377_0x630_sci_1_Quality_Truth.pvl> /dev/null;


