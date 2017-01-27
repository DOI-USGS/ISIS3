# Added Kristin Berry 2017-12-21 for new test data and significant updates to tgocassisi2isis. 
# References # 4584

APPNAME = tgocassis2isis

include $(ISISROOT)/make/isismake.tsts

commands: 
# TEST: Throws an error for a bad Spacecraft name
	if [ `$(APPNAME) \
	  from=$(INPUT)/CAS-MCO-2016-11-26T22.35.51.907-RED-01033-B1-BROKEN.xml \
	  to=$(OUTPUT)/BROKEN.cub \
	  2>> $(OUTPUT)/error.txt > /dev/null` ]; \
	  then true; \
	  fi; 
# Cleanup
	$(RM) $(OUTPUT)/BROKEN.cub; 
     

