APPNAME = mvic2isis

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/mc3_0034948318_0x536_sci_1.fit \
          to=$(OUTPUT)/mc3_0034948318_0x536_sci_1_Truth.cub \
          > /dev/null;
	catlab from=$(OUTPUT)/mc3_0034948318_0x536_sci_1_Truth.cub > \
          $(OUTPUT)/mc3_0034948318_0x536_sci_1_Truth.pvl;

