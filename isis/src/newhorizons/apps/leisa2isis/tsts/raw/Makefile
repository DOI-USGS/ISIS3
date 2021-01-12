APPNAME = leisa2isis

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/lsb_0030594839_0x53d_eng_1.fit \
          to=$(OUTPUT)/lsb_0030594839_0x53d_eng_1_Truth.cub \
          > /dev/null;
	catlab from=$(OUTPUT)/lsb_0030594839_0x53d_eng_1_Truth.cub > \
	  $(OUTPUT)/lsb_0030594839_0x53d_eng_1_Truth.pvl;
	$(APPNAME) from=$(INPUT)/lsb_0030594839_0x53d_eng_1.fit \
	  to=$(OUTPUT)/should_not_exist_1.cub \
	  error=$(OUTPUT)/should_not_exist_2.cub \
	  2>&1 | sed -e 's|\[[^]]*/\(.*\)\]|\[\1\]|g' > $(OUTPUT)/error1.txt || true;
	$(APPNAME) from=$(INPUT)/lsb_0030594839_0x53d_eng_1.fit \
	  to=$(OUTPUT)/should_not_exist_3.cub \
	  quality=$(OUTPUT)/should_not_exist_4.cub \
	  2>&1 | sed -e 's|\[[^]]*/\(.*\)\]|\[\1\]|g' > $(OUTPUT)/error2.txt || true;

