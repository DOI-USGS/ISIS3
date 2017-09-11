# Application test for Juno JunoCam ingestion.
#
# @history 2017-08-21 Jesse Mapel - Original version. 
#
APPNAME = junocam2isis
IMAGEBASE = JNCE_2016240_01C06147_V01

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/$(IMAGEBASE).LBL \
	           to=$(OUTPUT)/$(IMAGEBASE).cub \
	           >& /dev/null;
# Rename the output list as txt files so that the test system compares them
	mv $(OUTPUT)/$(IMAGEBASE).lis \
	   $(OUTPUT)/$(IMAGEBASE).txt;
	mv $(OUTPUT)/$(IMAGEBASE)_BLUE.lis \
	   $(OUTPUT)/$(IMAGEBASE)_BLUE.txt;
	mv $(OUTPUT)/$(IMAGEBASE)_GREEN.lis \
	   $(OUTPUT)/$(IMAGEBASE)_GREEN.txt;
	mv $(OUTPUT)/$(IMAGEBASE)_RED.lis \
	   $(OUTPUT)/$(IMAGEBASE)_RED.txt;