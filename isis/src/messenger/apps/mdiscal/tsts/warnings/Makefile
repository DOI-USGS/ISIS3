# This tests that the program prints warnings, when appropriate.
# Note that 2-4 fail, probably because the pixel bin value was simply edited in a cube that 
# originally has pixbin=0 rather than finding a test image from PDS that fits this criteria.
APPNAME = mdiscal

include $(ISISROOT)/make/isismake.tsts

commands:
	echo -e "\nTEST WARNING: DarkCurrent=Model and exposure > 1 sec" &> $(OUTPUT)/warnings.txt;
	if [[ `$(APPNAME) \
	FROM=$(INPUT)/EW0089565626A_modifiedExposure.cub \
	to=$(OUTPUT)/model_longExposure.cub \
	DARKCURRENT=model \
	2>> $(OUTPUT)/warnings.txt \
	> /dev/null` ]]; \
	then true; \
	fi;
	$(RM) $(OUTPUT)/model_longExposure.cub;


	echo -e "\nTEST WARNING: DarkCurrent=Linear and no valid dark." >> $(OUTPUT)/warnings.txt;
	if [[ `$(APPNAME) \
	FROM=$(INPUT)/EW0089565626A_modifiedExposureAndPixelBin.cub \
	to=$(OUTPUT)/linear.cub \
	DARKCURRENT=Linear \
	2>> $(OUTPUT)/warnings.txt \
	> /dev/null` ]]; \
	then true; \
	fi;


	echo -e "\nTEST WARNING: DarkCurrent=Linear and no valid dark." >> $(OUTPUT)/warnings.txt;
	if [[ `$(APPNAME) \
	  FROM=$(INPUT)/EW0089565626A_modifiedExposureAndPixelBin.cub \
	  to=$(OUTPUT)/standard.cub \
	  DARKCURRENT=standard \
	  2>> $(OUTPUT)/warnings.txt \
	  > /dev/null` ]]; \
	then true; \
	fi;


	echo -e "\nTEST WARNING: DarkCurrent=Model, exposure > 1, no valid dark." \
	>> $(OUTPUT)/warnings.txt;
	if [[ `$(APPNAME) \
	  FROM=$(INPUT)/EW0089565626A_modifiedExposureAndPixelBin.cub \
	  to=$(OUTPUT)/model.cub \
	  DARKCURRENT=model \
	  2>> $(OUTPUT)/warnings.txt \
	  > /dev/null` ]]; \
	then true; \
	fi;


	echo -e "\nFOR TEST COVERAGE: DarkCurrent=None and no valid dark." >> $(OUTPUT)/warnings.txt;
	if [[ `$(APPNAME) \
	  FROM=$(INPUT)/EW0089565626A_modifiedExposureAndPixelBin.cub \
	  to=$(OUTPUT)/nodark.cub \
	  DARKCURRENT=none \
	  2>> $(OUTPUT)/warnings.txt \
	  > /dev/null` ]]; \
	then true; \
	fi;


	$(RM) ./MDISWAC_NOTBIN_FLAT*.cub;
