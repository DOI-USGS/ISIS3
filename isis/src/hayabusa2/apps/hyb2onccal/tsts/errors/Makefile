# Test for hayabusa 2 ONC ingestion errors
#
# @history 2017-07-06 Jeannie Backer and Ian Humphrey - Original version. 
#
APPNAME = hyb2onccal

include $(ISISROOT)/make/isismake.tsts

commands:
	#   TEST: pass in cube with multiple bands
	echo -e "Test multiple bands:" > $(OUTPUT)/error_temp.txt;
	if [[ `$(APPNAME) \
	  from=$(INPUT)/multipleBands.cub \
	  to=$(OUTPUT)/output.cub  \
	  2>> $(OUTPUT)/error_temp.txt \
	  > /dev/null` ]]; \
	then \
	  true; \
	fi;
	#   TEST: pass in cube without ExposureDuration
	echo -e "\nTest missing ExposureDuration:" >> $(OUTPUT)/error_temp.txt;
	if [[ `$(APPNAME) \
	  from=$(INPUT)/noExposureDuration.cub \
	  to=$(OUTPUT)/output.cub  \
	  2>> $(OUTPUT)/error_temp.txt \
	  > /dev/null` ]]; \
	then \
	  true; \
	fi;
	#   TEST: pass in cube without CcdTemperature
	echo -e "\nTest missing CcdTemperature:" >> $(OUTPUT)/error_temp.txt;
	if [[ `$(APPNAME) \
	  from=$(INPUT)/noCcdTemperature.cub \
	  to=$(OUTPUT)/output.cub  \
	  2>> $(OUTPUT)/error_temp.txt \
	  > /dev/null` ]]; \
	then \
	  true; \
	fi;

	$(SED) 's+\[/.*/input/+\[input/+' $(OUTPUT)/error_temp.txt > $(OUTPUT)/error.txt;
	$(RM) $(OUTPUT)/error_temp.txt;
	$(RM) $(OUTPUT)/output.cub;
