APPNAME = mrf2pds

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/Level3.cub \
	to=$(OUTPUT)/Level3_OUT \
	userlbl=$(INPUT)/UserLabels.pvl > /dev/null;	

	$(MV) $(OUTPUT)/Level3_OUT.lbl $(OUTPUT)/Level3_OUT.pvl > /dev/null;	

