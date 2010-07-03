APPNAME = mosrange

mosrange_equi.txt.IGNORELINES = MinPixelResolution MaxPixelResolution \
                   PreciseCenterLongitude PreciseCenterLatitude \
                   PreciseMinimumLatitude PreciseMaximumLatitude \
                   PreciseMinimumLongitude PreciseMaximumLongitude


include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) FROMLIST=$(INPUT)/files.lis \
	MAP=$(ISIS3DATA)/base/templates/maps/equirectangular.map \
	TO=$(OUTPUT)/mosrange_equi.map PRECISION=4 > /dev/null;
	/bin/mv $(OUTPUT)/mosrange_equi.map $(OUTPUT)/mosrange_equi.txt
