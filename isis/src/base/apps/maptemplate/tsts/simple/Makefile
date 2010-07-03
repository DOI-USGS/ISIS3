APPNAME = maptemplate

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) projection=SimpleCylindrical Rngopt=user minlat=0 maxlat=180 minlon=0 maxlon=180 \
	  clon=0.0 map=$(OUTPUT)/maptemplateTruth.txt > /dev/null;
