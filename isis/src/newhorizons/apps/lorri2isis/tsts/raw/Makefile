APPNAME = lorri2isis

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/lor_0035140199_0x630_eng_1.fit \
	  to=$(OUTPUT)/lor_0035140199_0x630_eng_1.cub \
	  > /dev/null;
	catlab from=$(OUTPUT)/lor_0035140199_0x630_eng_1.cub \
	  to=$(OUTPUT)/lor_0035140199_0x630_eng_1.pvl> /dev/null;


