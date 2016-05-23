APPNAME = autoseed

include $(ISISROOT)/make/isismake.tsts

commands:
	# Test seeding two MOC WA images using a Grid seeder
	$(LS) $(INPUT)/*sub.cub > $(OUTPUT)/sub.lis;
	$(APPNAME) fromlist=$(OUTPUT)/sub.lis \
	           deffile=$(INPUT)/grid.pvl \
	           overlaplist=$(INPUT)/suboverlaps.def \
	           onet=$(OUTPUT)/sub.net \
	           networkid=SubNetwork \
	           pointid="sub????" \
	           description=SubNetwork \
	           >& /dev/null;
	# Test a denser grid and add it to the existing one created above
	$(LS) $(INPUT)/*.cub > $(OUTPUT)/list.lis;
	$(APPNAME) fromlist=$(OUTPUT)/list.lis \
	           deffile=$(INPUT)/smallgrid.pvl \
	           overlaplist=$(INPUT)/listoverlaps.def \
	           cnet=$(OUTPUT)/sub.net \
	           onet=$(OUTPUT)/new.net \
	           networkid=NewNetwork \
	           pointid="new????" \
	           description=NewNetwork \
	           errors=errors.txt \
	           >& /dev/null;
	$(RM) $(OUTPUT)/list.lis;
	$(RM) $(OUTPUT)/sub.lis;
