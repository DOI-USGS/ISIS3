# This test will print errors thrown by the pixel2map application.
# @internal
#   @history 2016-12-09 Curtis Rose - Original Version.
#                           References #4535.

APPNAME = pixel2map

include $(ISISROOT)/make/isismake.tsts

commands:
#   TEST A: Test that an exception is thrown when the cube list is empty.
	echo -e "Error Test A:" > $(OUTPUT)/error_temp.txt;
	touch $(OUTPUT)/emptyList.lis;
	if [[ ` $(APPNAME) fromtype=fromlist \
		fromlist=$(OUTPUT)/emptyList.lis \
		to=$(OUTPUT)/output.cub \
		2>> $(OUTPUT)/error_temp.txt \
		> /dev/null` ]]; \
	then \
	  true; \
	fi;
	$(RM) $(OUTPUT)/emptyList.lis;
#   TEST B: Test that an exception is thrown when the cube has an Instrument targetName is the Sky.
	echo -e "Error Test B:" >> $(OUTPUT)/error_temp.txt;
	if [[ ` $(APPNAME) from=$(INPUT)/targetsky.cub \
		to=$(OUTPUT)/output.cub \
		2>> $(OUTPUT)/error_temp.txt \
		> /dev/null` ]]; \
	then \
	  true; \
	fi;
#   TEST C: Test that an exception is thrown when cubes in the list have different band dimensions.
	echo -e "Error Test C:" >> $(OUTPUT)/error_temp.txt;
	$(LS) $(INPUT)/CM_1540484927_1_001.ir.cub | xargs -I "{}" echo "{}+2" > $(OUTPUT)/files.lis;
	$(LS) $(INPUT)/CM_1540484927_1_002.ir.cub >> $(OUTPUT)/files.lis;
	$(LS) $(INPUT)/CM_1540484927_1_003.ir.cub >> $(OUTPUT)/files.lis;
	if [[ ` $(APPNAME) fromtype=fromlist \
		fromlist=$(OUTPUT)/files.lis \
		to=$(OUTPUT)/output.cub \
		2>> $(OUTPUT)/error_temp.txt \
		> /dev/null` ]]; \
	then \
	  true; \
	fi;
	$(RM) $(OUTPUT)/files.lis;
	$(RM) $(OUTPUT)/output-count-.cub;
	$(RM) $(OUTPUT)/output.cub;
#   TEST D: Test that an exception is thrown when an image spans the 0-360 longitude seam.
	echo -e "Error Test D:" >> $(OUTPUT)/error_temp.txt;
	if [[ ` $(APPNAME) from=$(INPUT)/VIR_b75_lv1.cub \
		to=$(OUTPUT)/output.cub \
		lonseam=error \
		2>> $(OUTPUT)/error_temp.txt \
		> /dev/null` ]]; \
	then \
	  true; \
	fi;
	
	$(SED) 's+\[/.*/input/+\[input/+' $(OUTPUT)/error_temp.txt > $(OUTPUT)/error.txt;
	$(RM) $(OUTPUT)/error_temp.txt;