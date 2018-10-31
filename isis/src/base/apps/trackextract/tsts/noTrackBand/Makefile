# Verify that a cube with no tracking band will cause an exception to be thrown.
#
# history 2018-07-27 Kaitlyn Lee - Original version.

APPNAME = trackextract

include $(ISISROOT)/make/isismake.tsts

commands:
	# TEST: Throws an error when trying to open a cube without a tracking band
	if [ `$(APPNAME) from=$(INPUT)/mosaic.cub \
				to=$(OUTPUT)/outputMosaic \
				2> $(OUTPUT)/temp.txt > /dev/null` ]; \
				then true; \
	fi;

	# Removes input file path up until input
	$(SED) 's+\[.*/input+[input+' $(OUTPUT)/temp.txt > $(OUTPUT)/errorTruth.txt;
	$(RM) $(OUTPUT)/temp.txt
