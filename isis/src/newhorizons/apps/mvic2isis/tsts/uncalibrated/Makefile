APPNAME = mvic2isis

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/mc1_0034942918_0x536_eng_1.fit \
          to=$(OUTPUT)/mc1_0034942918_0x536_eng_1_Truth.cub \
          > /dev/null;
	catlab from=$(OUTPUT)/mc1_0034942918_0x536_eng_1_Truth.cub > \
          $(OUTPUT)/mc1_0034942918_0x536_eng_1_Truth.pvl;

