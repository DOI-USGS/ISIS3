APPNAME = mrf2pds
PDS2ISIS= pds2isis

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/FSR_CDR_LV2_01921_0R_Lev2.cub \
	to=$(OUTPUT)/FSR_CDR_LV2_01921_0R_Lev2_OUT \
	userlbl=$(INPUT)/UserLabels.pvl \
	type=list src=$(INPUT)/Src.lis > /dev/null;
	
	$(PDS2ISIS) from=$(OUTPUT)/FSR_CDR_LV2_01921_0R_Lev2_OUT.lbl \
	image=$(OUTPUT)/FSR_CDR_LV2_01921_0R_Lev2_OUT.img \
	to=$(OUTPUT)/FSR_CDR_LV2_01921_0R_Lev2_OUT.cub > /dev/null;

	$(MV) $(OUTPUT)/FSR_CDR_LV2_01921_0R_Lev2_OUT.lbl $(OUTPUT)/FSR_CDR_LV2_01921_0R_Lev2_OUT.pvl > /dev/null;
	
	$(RM) $(OUTPUT)/FSR_CDR_LV2_01921_0R_Lev2_OUT.img > /dev/null;
	 


