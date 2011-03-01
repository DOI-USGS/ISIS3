APPNAME = mrf2pds

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/FSR_CDR_LV2_01921_0R_Lev2.cub \
	to=$(OUTPUT)/FSR_CDR_LV2_01921_0R_Lev2_OUT \
	userlbl=$(INPUT)/UserLabels.pvl \
	type=list src=$(INPUT)/Src.lis > /dev/null;	

	$(MV) $(OUTPUT)/FSR_CDR_LV2_01921_0R_Lev2_OUT.lbl $(OUTPUT)/FSR_CDR_LV2_01921_0R_Lev2_OUT.pvl > /dev/null;	
	 


