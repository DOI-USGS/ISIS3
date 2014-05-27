# run spiceinit with multiple ck database files in a config file
# MRO CRISM - 
#   1 ordinary ck
#   1 gimble
APPNAME = spiceinit

include $(ISISROOT)/make/isismake.tsts

commands:
	$(CP) $(INPUT)/frt0001cfd8_07_if124s_trr3_b24.cub $(OUTPUT)/spiceinitTruth.cub > /dev/null;
	$(APPNAME) from=$(OUTPUT)/spiceinitTruth.cub > /dev/null;
	catlab from=$(OUTPUT)/spiceinitTruth.cub to=$(OUTPUT)/label.pvl > /dev/null;
	$(SED) 's+mro/kernels/sclk/MRO_SCLKSCET.*tsc+mro/kernels/sclk/MRO_SCLKSCET.tsc+g' $(OUTPUT)/label.pvl > $(OUTPUT)/labels.pvl;
	rm $(OUTPUT)/spiceinitTruth.cub;
	rm $(OUTPUT)/label.pvl;
