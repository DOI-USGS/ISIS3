APPNAME = leisa2isis

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/lsb_0034931099_0x53c_sci_1.fit \
	  to=$(OUTPUT)/lsb_0034931099_0x53c_sci_1.fit.cub \
	  replace=yes > /dev/null; 
	catlab from=$(OUTPUT)/lsb_0034931099_0x53c_sci_1.fit.cub \
	  to=$(OUTPUT)/lsb_0034931099_0x53c_sci_1.fit.pvl> /dev/null


