APPNAME  = mapmos

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/ab102401_noMinLat.lev2.cub \
	  mosaic=$(OUTPUT)/mapmosAllExtents.cub \
	  create=true \
	  minlat=10.765 \
	  maxlat=34.445 \
	  minlon=219.720 \
	  maxlon=236.190 \
	  null=true > /dev/null;
	$(APPNAME) from=$(INPUT)/ab102401_noMinLat.lev2.cub \
	  mosaic=$(OUTPUT)/mapmosOneExtent.cub \
	  create=true \
	  minlat=10.765 \
	  null=true > /dev/null;
	if [ `$(APPNAME) from=$(INPUT)/ab102401_noMinLat.lev2.cub \
	  mosaic=$(OUTPUT)/mapmosNoExtents.cub \
	  create=true \
	  null=true >& $(OUTPUT)/error.txt` ]; \
        then \
	  true; \
        fi;
