# Test the handling of data file extensions
#
# @history 2018-02-24 Summer Stapleton - Original version
#
# NOTE On re-import of data from tgocassisrdrgen, data files will have a .img extension as
#	opposed to a .dat

APPNAME = tgocassis2isis

include $(ISISROOT)/make/isismake.tsts

commands:
# TEST This tests the .dat file extension
	$(APPNAME) from=$(INPUT)/CAS-MCO-2016-11-22T15.45.50.984-PAN-00000-B1.xml \
		to=$(OUTPUT)/CAS-MCO-2016-11-22T15.45.50.984-PAN-00000-B1.cub > /dev/null;
	
	catlab from=$(OUTPUT)/CAS-MCO-2016-11-22T15.45.50.984-PAN-00000-B1.cub \
		to=$(OUTPUT)/datLabels.pvl > /dev/null;
	
	
# TEST This tests the .img file extension
	$(APPNAME) from=$(INPUT)/CAS-MCO-2016-11-22T15.45.50.984-BLU-03000-B1.xml \
		to=$(OUTPUT)/CAS-MCO-2016-11-22T15.45.50.984-BLU-03000-B1.cub> /dev/null;
	
	catlab from=$(OUTPUT)/CAS-MCO-2016-11-22T15.45.50.984-BLU-03000-B1.cub \
		to=$(OUTPUT)/imgLabels.pvl > /dev/null;
	
	
# TEST This tests for the nonexistance of a .dat or .img file for a given .xml
	echo "Test for non-existant data file:" > $(OUTPUT)/errors.txt;
	if [ `$(APPNAME) \
		from=$(INPUT)/CAS-MCO-2016-11-22T15.45.50.984-RED-01000-B1.xml \
		to=$(OUTPUT)/BROKEN.cub \
		2>> $(OUTPUT)/errors_temp.txt > /dev/null` ]; \
		then true; \
		fi;

# Remove everything in brackets like filenames/paths from error messages
	$(SED) 's/\[\([^"]*\)\]//g' $(OUTPUT)/errors_temp.txt \
	> $(OUTPUT)/errors.txt;
	
# Cleanup
	$(RM) $(OUTPUT)/errors_temp.txt
	$(RM) $(OUTPUT)/BROKEN.cub; 
