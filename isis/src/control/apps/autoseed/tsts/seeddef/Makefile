APPNAME = autoseed

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) $(INPUT)/*.cub > $(OUTPUT)/list.lis;
	$(APPNAME) fromlist=$(OUTPUT)/list.lis \
	seeddef=$(INPUT)/gridPixels.pvl \
	overlaplist=$(INPUT)/listoverlaps.def \
	to=$(OUTPUT)/new.pixels.net.pvl \
	networkid=NewNetwork pointid="new????" description=NewNetwork \
	> /dev/null;
	$(APPNAME) fromlist=$(OUTPUT)/list.lis \
	seeddef=$(INPUT)/gridEmission.pvl \
	overlaplist=$(INPUT)/listoverlaps.def \
	to=$(OUTPUT)/new.emission.net.pvl \
	networkid=NewNetwork pointid="new????" description=NewNetwork \
	> /dev/null;
	$(APPNAME) fromlist=$(OUTPUT)/list.lis \
	seeddef=$(INPUT)/gridIncidence.pvl \
	overlaplist=$(INPUT)/listoverlaps.def \
	to=$(OUTPUT)/new.incidence.net.pvl \
	networkid=NewNetwork pointid="new????" description=NewNetwork \
	> /dev/null;
	$(APPNAME) fromlist=$(OUTPUT)/list.lis \
	seeddef=$(INPUT)/gridResolution.pvl \
	overlaplist=$(INPUT)/listoverlaps.def \
	to=$(OUTPUT)/new.resolution.net.pvl \
	networkid=NewNetwork pointid="new????" description=NewNetwork \
	> /dev/null;
	$(APPNAME) fromlist=$(OUTPUT)/list.lis \
	seeddef=$(INPUT)/gridDN.pvl \
	overlaplist=$(INPUT)/listoverlaps.def \
	to=$(OUTPUT)/new.dn.net.pvl \
	networkid=NewNetwork pointid="new????" description=NewNetwork \
	> /dev/null;
	$(RM) $(OUTPUT)/list.lis;
