APPNAME = mvic2isis

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/mc3_0034948318_0x536_sci_1.fit \
          to=$(OUTPUT)/mc3_0034948318_0x536_sci_1_Truth.cub \
          > /dev/null;
	catlab from=$(OUTPUT)/mc3_0034948318_0x536_sci_1_Truth.cub > \
          $(OUTPUT)/mc3_0034948318_0x536_sci_1_Truth.pvl;
	$(APPNAME) from=$(INPUT)/mc3_0034948318_0x536_sci_1.fit \
          to=$(OUTPUT)/mc3_0034948318_0x536_sci_1_Truth2.cub \
	  undistorted=$(OUTPUT)/mc3_0034948318_0x536_sci_1_Undistorted \
	  error=$(OUTPUT)/mc3_0034948318_0x536_sci_1_Error \
	  quality=$(OUTPUT)/mc3_0034948318_0x536_sci_1_Quality \
          > /dev/null;
	catlab from=$(OUTPUT)/mc3_0034948318_0x536_sci_1_Truth2.cub > \
	  $(OUTPUT)/mc3_0034948318_0x536_sci_1_Truth2.pvl;
	catlab from=$(OUTPUT)/mc3_0034948318_0x536_sci_1_Undistorted.cub > \
	  $(OUTPUT)/mc3_0034948318_0x536_sci_1_Undistorted.pvl;
	catlab from=$(OUTPUT)/mc3_0034948318_0x536_sci_1_Error.cub > \
	  $(OUTPUT)/mc3_0034948318_0x536_sci_1_Error.pvl;
	catlab from=$(OUTPUT)/mc3_0034948318_0x536_sci_1_Quality.cub > \
	  $(OUTPUT)/mc3_0034948318_0x536_sci_1_Quality.pvl;
