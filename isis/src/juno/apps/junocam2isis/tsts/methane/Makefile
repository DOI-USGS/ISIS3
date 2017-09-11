# Application test for Juno JunoCam ingestion.
#
# @history 2017-08-21 Jesse Mapel - Original version. 
#
APPNAME = junocam2isis
IMAGEBASE = JNCE_2016240_01M06148_V01

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/$(IMAGEBASE).LBL \
	           to=$(OUTPUT)/$(IMAGEBASE).cub \
	           >& /dev/null;
# Rename the output lists as txt files so that the test system compares them
	mv $(OUTPUT)/$(IMAGEBASE).lis \
	   $(OUTPUT)/$(IMAGEBASE).txt;
	mv $(OUTPUT)/$(IMAGEBASE)_METHANE.lis \
	   $(OUTPUT)/$(IMAGEBASE)_METHANE.txt;