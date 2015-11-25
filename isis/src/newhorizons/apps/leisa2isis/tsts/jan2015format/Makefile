APPNAME = leisa2isis

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/lsb_0034931099_0x53c_sci_1.fit \
          to=$(OUTPUT)/lsb_0034931099_0x53c_sci_1_Truth.cub \
          > /dev/null;
	catlab from=$(OUTPUT)/lsb_0034931099_0x53c_sci_1_Truth.cub > \
	  $(OUTPUT)/lsb_0034931099_0x53c_sci_1_Truth.pvl;
	$(APPNAME) from=$(INPUT)/lsb_0034931099_0x53c_sci_1.fit \
	  to=$(OUTPUT)/lsb_0034931099_0x53c_sci_1_Truth.cub \
	  error=$(OUTPUT)/lsb_0034931099_0x53c_sci_1_Error.cub \
	  > /dev/null;
	catlab from=$(OUTPUT)/lsb_0034931099_0x53c_sci_1_Error.cub > \
	  $(OUTPUT)/lsb_0034931099_0x53c_sci_1_Error.pvl;
	$(APPNAME) from=$(INPUT)/lsb_0034931099_0x53c_sci_1.fit \
	  to=$(OUTPUT)/lsb_0034931099_0x53c_sci_1_Truth.cub \
	  quality=$(OUTPUT)/lsb_0034931099_0x53c_sci_1_Quality.cub\
	  > /dev/null;
	catlab from=$(OUTPUT)/lsb_0034931099_0x53c_sci_1_Quality.cub > \
	  $(OUTPUT)/lsb_0034931099_0x53c_sci_1_Quality.pvl;


