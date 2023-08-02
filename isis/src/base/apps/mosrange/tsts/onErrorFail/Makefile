APPNAME = mosrange
# This test exercises mosrange error handling functionality. The ONERROR string is set to
# FAIL which is a directive to stop processing cubes as soon as any fail. Here, one of
# the input cube files (EN0108828327M_noSPICE.cub) has not been initialized with SPICE,
# which should trigger an error. A pvl log file with any errors and a file listing failed
# cube filenames are output. The $(CAT) $(OUTPUT)/errorList command removes cube filename
# paths. Note the dash before the ISIS command indicates to make that it's ok for this
# command to fail (which it's meant to do for this test). Make will continue after the
# failure. 
# 
# 2023-07-25 Ken Edmundson

mosrange_equi.txt.IGNORELINES = MinPixelResolution MaxPixelResolution \
                   PreciseCenterLongitude PreciseCenterLatitude \
                   PreciseMinimumLatitude PreciseMaximumLatitude \
                   PreciseMinimumLongitude PreciseMaximumLongitude


include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) $(INPUT)/*.cub > $(OUTPUT)/files.lis;
	-$(APPNAME) FROMLIST=$(OUTPUT)/files.lis \
	MAP=$(ISISROOT)/appdata/templates/maps/equirectangular.map \
	TO=$(OUTPUT)/mosrange_equi.map PRECISION=4 \
	ONERROR=fail \
	ERRORLOG=$(OUTPUT)/errorLog \
	ERRORLIST=$(OUTPUT)/errorList > /dev/null;
	$(MV) $(OUTPUT)/errorLog.log $(OUTPUT)/errorLog.pvl;
	$(CAT) $(OUTPUT)/errorList \
               | perl -pe 's/(^|,|: )([^,:]+\/)([^,\/:]*\.)(cub)/\1\3\4/g' 2>/dev/null \
               > $(OUTPUT)/errorList.txt;
	$(RM) $(OUTPUT)/errorList > /dev/null;
	$(RM) $(OUTPUT)/files.lis print.prt > /dev/null;
