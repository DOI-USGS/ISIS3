APPNAME = tgocassisrdrgen

include $(ISISROOT)/make/isismake.tsts

commands:
# TEST: Throws an error for a non-cube input
	echo "Test Non-Cube:" > $(OUTPUT)/errors.txt;
	if [ `$(APPNAME) \
	  from=$(INPUT)/NonCube.xml \
	  to=$(OUTPUT)/BROKEN.img \
	  2>> $(OUTPUT)/errors_temp.txt > /dev/null` ]; \
	  then true; \
	  fi; 
# TEST: Throws an error for a bad instrument name
	echo "" >> $(OUTPUT)/errors.txt ;
	echo "Test InstrumentId:" >> $(OUTPUT)/errors.txt ;
	if [ `$(APPNAME) \
	  from=$(INPUT)/InstrumentError.cub \
	  to=$(OUTPUT)/BROKEN.img \
	  2>> $(OUTPUT)/errors_temp.txt > /dev/null` ]; \
	  then true; \
	  fi;
# Remove everything in brackets like filenames/paths from error messages
	$(SED) 's/\[\([^"]*\)\]//g' $(OUTPUT)/errors_temp.txt \
	> $(OUTPUT)/errors.txt; 

# Cleanup
	$(RM) $(OUTPUT)/errors_temp.txt;
	$(RM) $(OUTPUT)/BROKEN.img;	
