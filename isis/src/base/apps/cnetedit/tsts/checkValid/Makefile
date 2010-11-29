APPNAME = cnetedit

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) $(INPUT)/*.cub > $(INPUT)/list.lis;
	$(APPNAME) CNET=$(INPUT)/cnet.net \
	ONET=$(OUTPUT)/cnet.pvl \
	LOG=$(OUTPUT)/log.txt \
	CHECKVALID=yes \
       	FROMLIST=$(INPUT)/list.lis \
	DEFFILE=$(INPUT)/deffile.def > /dev/null;
	$(RM) $(INPUT)/list.lis;
