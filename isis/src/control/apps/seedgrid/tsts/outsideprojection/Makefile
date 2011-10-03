# Tests trying to seed on a lat/lon range that is outside of the projection's
# min/max lat/lon. Should return an empty control network because none of the
# points are within the min/max lat/lon of the orthographic projection.
APPNAME = seedgrid

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) map=$(INPUT)/orthographic.map minlat=80 maxlat=90 \
	minlon=30 maxlon=68 xstep=100000 ystep=100000 \
	networkid=NET001 pointid=PT??????? description="Test control network" \
	onet=$(OUTPUT)/outnet.net >& /dev/null
