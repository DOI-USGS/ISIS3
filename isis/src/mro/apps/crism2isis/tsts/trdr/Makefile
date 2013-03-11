APPNAME = crism2isis

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/frt00003b73_01_if156s_trr2.lbl \
	to=$(OUTPUT)/trdrtruth.cub > /dev/null;
	$(APPNAME) from=$(INPUT)/frt0001e5c3_07_if124s_trr3.lbl \
	to=$(OUTPUT)/trdrtruth2.cub > /dev/null;
	getkey from=$(OUTPUT)/trdrtruth2.cub grp=Kernels keyword=NaifIkCode > $(OUTPUT)/ik.txt;
