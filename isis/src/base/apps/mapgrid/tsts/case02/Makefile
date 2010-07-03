APPNAME = mapgrid

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) mapfile=$(INPUT)/mapFile2.txt \
	  startlat=20 \
	  endlat=60 \
	  startlon=0 \
	  endlon=40 \
	  latspacing=25 \
	  lonspacing=25 \
	  latinc=5 \
	  loninc=5 \
	  to=$(OUTPUT)/mapgridTruth2.txt > /dev/null;
