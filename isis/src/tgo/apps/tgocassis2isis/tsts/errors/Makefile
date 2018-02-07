# Verify that input is valid TGO CaSSIS image.
#
# history 2016-12-21 Kristin Berry - Original version. References # 4584
# history 2017-09-18 Jeannie Backer - Added checks for Instrument and Filter.
# history 2017-11-26 Kristin Berry - Strip filenames out of output. 

APPNAME = tgocassis2isis

include $(ISISROOT)/make/isismake.tsts

commands: 
# TEST: Throws an error for a bad Spacecraft name
	echo "Test SpacecraftName:" > $(OUTPUT)/errors.txt;
	if [ `$(APPNAME) \
	  from=$(INPUT)/CAS-MCO-2016-11-26T22.35.51.907-RED-01033-B1-SpacecraftError.xml \
	  to=$(OUTPUT)/BROKEN.cub \
	  2>> $(OUTPUT)/errors_temp.txt > /dev/null` ]; \
	  then true; \
	  fi; 
# TEST: Throws an error for a bad instrument name
	echo "" >> $(OUTPUT)/errors.txt ;
	echo "Test InstrumentId:" >> $(OUTPUT)/errors.txt ;
	if [ `$(APPNAME) \
	  from=$(INPUT)/CAS-MCO-2016-11-26T22.35.51.907-RED-01033-B1-InstrumentError.xml \
	  to=$(OUTPUT)/BROKEN.cub \
	  2>> $(OUTPUT)/errors_temp.txt > /dev/null` ]; \
	  then true; \
	  fi; 
# TEST: Throws an error for a bad filter name
	echo "" >> $(OUTPUT)/errors.txt ;
	echo "Test FilterName:" >> $(OUTPUT)/errors.txt ;
	if [ `$(APPNAME) \
	  from=$(INPUT)/CAS-MCO-2016-11-20T15.30.00.349-DMP-00000-00.xml \
	  to=$(OUTPUT)/BROKEN.cub \
	  2>> $(OUTPUT)/errors_temp.txt > /dev/null` ]; \
	  then true; \
	  fi; 
# Remove everything in brackets like filenames/paths from error messages
	$(SED) 's/\[\([^"]*\)\]//g' $(OUTPUT)/errors_temp.txt \
	> $(OUTPUT)/errors.txt; 

# Cleanup
	$(RM) $(OUTPUT)/errors_temp.txt;
	$(RM) $(OUTPUT)/BROKEN.cub; 

     

