APPNAME = lorri2isis

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/lor_0030710290_0x633_sci_1.fit \
	  to=$(OUTPUT)/lor_0030710290_0x633_sci_1.cub \
	  > /dev/null;
	catlab from=$(OUTPUT)/lor_0030710290_0x633_sci_1.cub \
	  to=$(OUTPUT)/lor_0030710290_0x633_sci_1.pvl > /dev/null;


