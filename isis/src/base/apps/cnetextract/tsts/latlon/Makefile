APPNAME = cnetextract

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) $(INPUT)/*.cub > $(OUTPUT)/list.lis;
	$(APPNAME) fromlist=$(OUTPUT)/list.lis cnet=$(INPUT)/cnet.net \
	tolist=$(OUTPUT)/newList.lis outnet=$(OUTPUT)/newNet.pvl \
	networkid=new description=new prefix=$(OUTPUT)/out \
	latlon=true minlat=15.0 maxlat=28.0 minlon=20.0 maxlon=28.0 > /dev/null;
	$(RM) $(OUTPUT)/newList.lis;
	$(RM) $(OUTPUT)/list.lis;
