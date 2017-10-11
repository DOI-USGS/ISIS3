APPNAME = tgocassisrdrgen

include $(ISISROOT)/make/isismake.tsts

commands: 
	$(APPNAME) from=$(INPUT)/CAS-MCO-2016-11-26T22.32.39.582-BLU-03025-00.cub \
	to=$(OUTPUT)/CAS-MCO-2016-11-26T22.32.39.582-BLU-03025-00.img > /dev/null;
	
	# Reingest the PDS4 data
	raw2isis from=$(OUTPUT)/CAS-MCO-2016-11-26T22.32.39.582-BLU-03025-00.img \
	to=$(OUTPUT)/CAS-MCO-2016-11-26T22.32.39.582-BLU-03025-00.cub \
	SAMPLES=2048 \
	LINES=254 \
	BITTYPE=REAL \
	BYTEORDER=LSB > /dev/null;
	
	# Compare the output cube from raw2isis to the original input cube to
	# ensure that they are identical
	cubediff from=$(INPUT)/CAS-MCO-2016-11-26T22.32.39.582-BLU-03025-00.cub \
	from2=$(OUTPUT)/CAS-MCO-2016-11-26T22.32.39.582-BLU-03025-00.cub \
	to=$(OUTPUT)/cubediffresults.txt > /dev/null;
	          
	# Change the output xml file to a txt file for test comparison
	mv $(OUTPUT)/CAS-MCO-2016-11-26T22.32.39.582-BLU-03025-00.xml \
	$(OUTPUT)/CAS-MCO-2016-11-26T22.32.39.582-BLU-03025-00.txt > /dev/null;
