APPNAME = maptemplate

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) projection=Mercator  clon=0.0 clat=90.0 resopt=mpp resolution=1 \
	  map=$(OUTPUT)/maptemplateTruth.txt > /dev/null;
