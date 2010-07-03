APPNAME = maptemplate

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) projection=LambertConformal clon=0.0 clat=90.0 par1=45.0 par2=55.0 \
	  map=$(OUTPUT)/maptemplateTruth.txt > /dev/null;
