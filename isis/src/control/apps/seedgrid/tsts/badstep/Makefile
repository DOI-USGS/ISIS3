# This tests the case where the x step is larger than the longitude range. It
# should return an empty control network.
APPNAME = seedgrid

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) map=$(INPUT)/sinusoidal.map minlat=40 maxlat=50 \
	minlon=280 maxlon=282 xstep=100000000000 ystep=100 \
	networkid=NET001 pointid=PT?? description="Test control network" \
	onet=$(OUTPUT)/outnet.net >& /dev/null
