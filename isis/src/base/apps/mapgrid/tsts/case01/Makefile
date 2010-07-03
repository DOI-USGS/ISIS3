APPNAME = mapgrid

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) mapfile=$(INPUT)/mapFile1.txt \
	  startlat=-90 \
	  endlat=90 \
	  startlon=0 \
	  endlon=360 \
	  latspacing=25 \
	  lonspacing=25 \
	  latinc=5 \
	  loninc=5 \
	  to=$(OUTPUT)/mapgridTruth.txt > /dev/null;
