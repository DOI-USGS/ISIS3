APPNAME = cnetedit

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) $(INPUT)/*.cub > $(OUTPUT)/list.lis;

	$(APPNAME) CNET=$(INPUT)/cnet.net \
	LOG=$(OUTPUT)/locked.txt \
	CUBELIST=$(OUTPUT)/list.lis \
	POINTLIST=$(INPUT)/points.lis \
	IGNORE=no DELETE=no LOCK=yes \
	ONET=$(OUTPUT)/locked.net > /dev/null;

	$(APPNAME) CNET=$(INPUT)/cnet.net \
	LOG=$(OUTPUT)/unlocked.txt \
	CUBELIST=$(OUTPUT)/list.lis \
	POINTLIST=$(INPUT)/points.lis \
	UNLOCK=yes IGNORE=no DELETE=no \
	ONET=$(OUTPUT)/unlocked.net > /dev/null;

	$(RM) $(OUTPUT)/list.lis;
