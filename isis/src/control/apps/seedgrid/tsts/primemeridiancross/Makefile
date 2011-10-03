# Tests a longitude range that crosses the 0/360 boundary.
APPNAME = seedgrid

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) target=mars minlat=-90 maxlat=90 \
	minlon=300 maxlon=380 latstep=20 lonstep=20 spacing=latlon \
	networkid=NET001 pointid=PT??? description="Test control network" \
	onet=$(OUTPUT)/outnet.net \
	> /dev/null;
