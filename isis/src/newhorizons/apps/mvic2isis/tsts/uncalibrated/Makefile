APPNAME = mvic2isis

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/mc1_0034942918_0x536_eng_1.fit \
          to=$(OUTPUT)/mc1_0034942918_0x536_eng_1_Truth.cub \
          > /dev/null;
	catlab from=$(OUTPUT)/mc1_0034942918_0x536_eng_1_Truth.cub > \
	  $(OUTPUT)/mc1_0034942918_0x536_eng_1_Truth.pvl;
	$(APPNAME) from=$(INPUT)/mc1_0034942918_0x536_eng_1.fit \
	  to=$(OUTPUT)/should_not_exist_1.cub \
	  undistorted=$(OUTPUT)/should_not_exist_2.cub \
	  2>&1 | sed -e 's|\[[^]]*/\(.*\)\]|\[\1\]|g' > $(OUTPUT)/error1.txt || true;
	$(APPNAME) from=$(INPUT)/mc1_0034942918_0x536_eng_1.fit \
	  to=$(OUTPUT)/should_not_exist_3.cub \
	  error=$(OUTPUT)/should_not_exist_4.cub \
	  2>&1 | sed -e 's|\[[^]]*/\(.*\)\]|\[\1\]|g' > $(OUTPUT)/error2.txt || true;
	$(APPNAME) from=$(INPUT)/mc1_0034942918_0x536_eng_1.fit \
	  to=$(OUTPUT)/should_not_exist_5.cub \
	  quality=$(OUTPUT)/should_not_exist_6.cub \
	  2>&1 | sed -e 's|\[[^]]*/\(.*\)\]|\[\1\]|g' > $(OUTPUT)/error3.txt || true;

