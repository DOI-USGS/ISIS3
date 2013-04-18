APPNAME = maptemplate

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) map=$(OUTPUT)/pointPerspectiveTruth.txt \
	projection=pointperspective clon=0 clat=0 dist=6000000 \
	targopt=user targetname=MARS rngopt=user minlat=47.4310794 \
	maxlat=49.323205 minlon=276.482503 maxlon=279.272070
	resopt=ppd resolution=512.0 > /dev/null;
