APPNAME = autoseed

include $(ISISROOT)/make/isismake.tsts

commands:
	# Get a file with a list of cubes in it
	$(LS) $(INPUT)/*.cub > $(OUTPUT)/list.lis;
	# Test
	$(APPNAME) fromlist=$(OUTPUT)/list.lis \
	deffile=$(INPUT)/gridPixels.pvl \
	overlaplist=$(INPUT)/listoverlaps.def \
	onet=$(OUTPUT)/new.pixels.net \
	networkid=NewNetwork pointid="new????" description=NewNetwork \
	> /dev/null;
	# Test
	$(APPNAME) fromlist=$(OUTPUT)/list.lis \
	deffile=$(INPUT)/gridEmission.pvl \
	overlaplist=$(INPUT)/listoverlaps.def \
	onet=$(OUTPUT)/new.emission.net \
	networkid=NewNetwork pointid="new????" description=NewNetwork \
	> /dev/null;
	# Test
	$(APPNAME) fromlist=$(OUTPUT)/list.lis \
	deffile=$(INPUT)/gridIncidence.pvl \
	overlaplist=$(INPUT)/listoverlaps.def \
	onet=$(OUTPUT)/new.incidence.net \
	networkid=NewNetwork pointid="new????" description=NewNetwork \
	> /dev/null;
	# Test
	$(APPNAME) fromlist=$(OUTPUT)/list.lis \
	deffile=$(INPUT)/gridResolution.pvl \
	overlaplist=$(INPUT)/listoverlaps.def \
	onet=$(OUTPUT)/new.resolution.net \
	networkid=NewNetwork pointid="new????" description=NewNetwork \
	> /dev/null;
	# Test
	$(APPNAME) fromlist=$(OUTPUT)/list.lis \
	deffile=$(INPUT)/gridDN.pvl \
	overlaplist=$(INPUT)/listoverlaps.def \
	onet=$(OUTPUT)/new.dn.net \
	networkid=NewNetwork pointid="new????" description=NewNetwork \
	> /dev/null;
	$(RM) $(OUTPUT)/list.lis;
