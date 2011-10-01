# Tests x- and y-step with a south polar projection.
APPNAME = seedgrid

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) map=$(INPUT)/polarstereographic.map minlat=-80 maxlat=-60 \
	minlon=30 maxlon=68 xstep=10000 ystep=10000 \
	networkid=NET001 pointid=PT???? description="Test control network" \
	onet=$(OUTPUT)/outnet.net >& /dev/null
