APPNAME = autoseed

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) $(INPUT)/*.cub > $(OUTPUT)/list.lis;
	$(LS) $(INPUT)/*sub.cub > $(OUTPUT)/sub.lis;
	$(APPNAME) fromlist=$(OUTPUT)/sub.lis \
	seeddef=$(INPUT)/grid.pvl \
	overlaplist=$(INPUT)/suboverlaps.def \
	to=$(OUTPUT)/sub.net.pvl \
	networkid=SubNetwork \
	pointid="sub????" \
	description=SubNetwork \
	> /dev/null;
	$(APPNAME) fromlist=$(OUTPUT)/list.lis \
	seeddef=$(INPUT)/smallgrid.pvl \
	overlaplist=$(INPUT)/listoverlaps.def \
	cnet=$(OUTPUT)/sub.net.pvl \
	to=$(OUTPUT)/new.net.pvl \
	networkid=NewNetwork \
	pointid="new????" \
	description=NewNetwork \
	> /dev/null;
	$(RM) $(OUTPUT)/list.lis;
	$(RM) $(OUTPUT)/sub.lis;
