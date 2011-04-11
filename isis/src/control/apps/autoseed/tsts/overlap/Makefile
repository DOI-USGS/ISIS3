APPNAME = autoseed

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) $(INPUT)/*.cub > $(OUTPUT)/list.lis;
	$(LS) $(INPUT)/*sub.cub > $(OUTPUT)/sub.lis;
	$(APPNAME) fromlist=$(OUTPUT)/sub.lis \
	deffile=$(INPUT)/grid.pvl \
	overlaplist=$(INPUT)/suboverlaps.def \
	onet=$(OUTPUT)/sub.net \
	networkid=SubNetwork \
	pointid="sub????" \
	description=SubNetwork \
	> /dev/null;
	$(APPNAME) fromlist=$(OUTPUT)/list.lis \
	deffile=$(INPUT)/smallgrid.pvl \
	overlaplist=$(INPUT)/listoverlaps.def \
	cnet=$(OUTPUT)/sub.net \
	onet=$(OUTPUT)/new.net \
	networkid=NewNetwork \
	pointid="new????" \
	description=NewNetwork \
	> /dev/null;
	$(RM) $(OUTPUT)/list.lis;
	$(RM) $(OUTPUT)/sub.lis;
