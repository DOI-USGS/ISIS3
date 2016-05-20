APPNAME = seedgrid

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) target=moon \
	           minlat=-10.6 \
	           maxlat=-7.3 \
	           spacing=latlon \
	           minlon=14.3 \
	           maxlon=16.6 \
	           latstep=0.35 \
	           lonstep=0.71 \
	           networkid=NET001 \
	           pointid=PT?? \
	           description="Test control network" \
	           onet=$(OUTPUT)/outnet.net \
	           > /dev/null;
