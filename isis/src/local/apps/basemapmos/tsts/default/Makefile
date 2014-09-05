APPNAME = basemapmos

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) $(INPUT)/* > $(OUTPUT)/cubes.lis;
	$(APPNAME) FROMLIST=$(OUTPUT)/cubes.lis \
	TO=$(OUTPUT)/result.cub \
	HNS1=73 HNL1=73 HNS2=103 HNL2=103 LNS=83 LNL=83 > /dev/null;
	$(APPNAME) FROMLIST=$(OUTPUT)/cubes.lis \
	TO=$(OUTPUT)/userrangeresult.cub \
	GRANGE=USER MINLAT=18 MAXLAT=37 MINLON=12 MAXLON=28 \
	HNS1=73 HNL1=73 HNS2=103 HNL2=103 LNS=83 LNL=83 > /dev/null;
	$(RM) $(OUTPUT)/cubes.lis;
