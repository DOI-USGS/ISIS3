# Test for hayabusa 2 ONC ingestion errors
#
# @history 2017-07-06 Jeannie Backer and Ian Humphrey - Original version. 
#
APPNAME = hyb2onc2isis

include $(ISISROOT)/make/isismake.tsts

commands:
	#   TEST: pass in file with invalid instrument name
	echo -e "Test invalid instrument:" > $(OUTPUT)/error_temp.txt;
	if [[ `$(APPNAME) \
	  from=$(INPUT)/badInst.fit \
	  to=$(OUTPUT)/output.cub  \
	  2>> $(OUTPUT)/error_temp.txt \
	  > /dev/null` ]]; \
	then \
	  true; \
	fi;
	#   TEST: pass in file with invalid spacecraft name
	echo -e "\nTest invalid spacecraft:" >> $(OUTPUT)/error_temp.txt;
	if [[ `$(APPNAME) \
	  from=$(INPUT)/badSpc.fit \
	  to=$(OUTPUT)/output.cub  \
	  2>> $(OUTPUT)/error_temp.txt \
	  > /dev/null` ]]; \
	then \
	  true; \
	fi;
	#   TEST: pass in file missing instrument name
	echo -e "\nTest missing instrument:" >> $(OUTPUT)/error_temp.txt;
	if [[ `$(APPNAME) \
	  from=$(INPUT)/noInst.fit \
	  to=$(OUTPUT)/output.cub  \
	  2>> $(OUTPUT)/error_temp.txt \
	  > /dev/null` ]]; \
	then \
	  true; \
	fi;
	#   TEST: pass in file missing spacecraft name
	echo -e "\nTest missing spacecraft:" >> $(OUTPUT)/error_temp.txt;
	if [[ `$(APPNAME) \
	  from=$(INPUT)/noSpc.fit \
	  to=$(OUTPUT)/output.cub  \
	  2>> $(OUTPUT)/error_temp.txt \
	  > /dev/null` ]]; \
	then \
	  true; \
	fi;
	#   TEST: pass invalid label format (i.e. no SIMPLE data section before main IMAGE data)
	echo -e "\nTest invalid label format:" >> $(OUTPUT)/error_temp.txt;
	if [[ `$(APPNAME) \
	  from=$(INPUT)/lor_0030710290_0x633_sci_1.fit \
	  to=$(OUTPUT)/output.cub  \
	  2>> $(OUTPUT)/error_temp.txt \
	  > /dev/null` ]]; \
	then \
	  true; \
	fi;

	$(SED) 's+\[/.*/input/+\[input/+' $(OUTPUT)/error_temp.txt > $(OUTPUT)/error.txt;
	$(RM) $(OUTPUT)/error_temp.txt;
	$(RM) $(OUTPUT)/output.cub;
