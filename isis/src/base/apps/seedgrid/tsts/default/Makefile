APPNAME = seedgrid

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) projection=$(INPUT)/sinusoidal.map minlat=40 maxlat=50 \
	minlon=280 maxlon=290 xstep=100000 ystep=100000 \
	networkid=NET001 pointid=PT?? description="Test control network" \
	to=$(OUTPUT)/outnet.pvl \
	> /dev/null;
