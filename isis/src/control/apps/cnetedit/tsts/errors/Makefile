APPNAME = cnetedit

include $(ISISROOT)/make/isismake.tsts

commands:
# Test with a non-existant file
	echo -e "Error Test: Non-existant file" > $(OUTPUT)/error_temp.txt;
	if [[ `$(APPNAME) CNET=$(INPUT)/cnet.net \
	  ONET=$(OUTPUT)/cnet.net \
	  2>> $(OUTPUT)/error_temp.txt \
	  > /dev/null` ]]; \
	then \
	  true; \
	fi;
	
	$(SED) 's+\[/.*/input/+\[input/+' $(OUTPUT)/error_temp.txt > $(OUTPUT)/error.txt;
	
# Cleanup
	$(RM) $(OUTPUT)/error_temp.txt;
