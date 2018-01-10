APPNAME = tgocassisrdrgen

include $(ISISROOT)/make/isismake.tsts

commands: 
	$(APPNAME) from=$(INPUT)/CAS-MCO-2016-11-26T22.32.39.582-BLU-03025-00.cub \
	           to=$(OUTPUT)/CAS-MCO-2016-11-26T22.32.39.582-BLU-03025-00.img \
		   > /dev/null;

	# Reingest the PDS4 data
	raw2isis from=$(OUTPUT)/CAS-MCO-2016-11-26T22.32.39.582-BLU-03025-00.img \
	         to=$(OUTPUT)/CAS-MCO-2016-11-26T22.32.39.582-BLU-03025-00.cub \
	         SAMPLES=2048 \
	         LINES=254 \
	         BITTYPE=REAL \
	         BYTEORDER=LSB \
		 > /dev/null;

	# Compare the output cube from raw2isis to the original input cube to
	# ensure that they are identical
	cubediff from=$(INPUT)/CAS-MCO-2016-11-26T22.32.39.582-BLU-03025-00.cub \
	         from2=$(OUTPUT)/CAS-MCO-2016-11-26T22.32.39.582-BLU-03025-00.cub \
	         to=$(OUTPUT)/cubediffresults.txt \
		 > /dev/null;

	$(SED) 's+\Product_Observational.*>+\Product_Observational>+' \
	       $(OUTPUT)/CAS-MCO-2016-11-26T22.32.39.582-BLU-03025-00.xml \
	       > $(OUTPUT)/templabel1.txt;
	$(SED) 's+\FSW_HEADER.*>+\FSW_HEADER>+' \
	       $(OUTPUT)/templabel1.txt \
	       > $(OUTPUT)/templabel2.txt;
	$(SED) 's+\PEHK_HEADER.*>+\PEHK_HEADER>+' \
	       $(OUTPUT)/templabel2.txt \
	       > $(OUTPUT)/CAS-MCO-2016-11-26T22.32.39.582-BLU-03025-00.xmlLabel.txt;

	$(RM) $(OUTPUT)/CAS-MCO-2016-11-26T22.32.39.582-BLU-03025-00.xml \
	      $(OUTPUT)/templabel1.txt \
	      $(OUTPUT)/templabel2.txt;
