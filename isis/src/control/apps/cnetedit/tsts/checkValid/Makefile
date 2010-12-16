APPNAME = cnetedit

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) $(INPUT)/*.cub > $(OUTPUT)/list.lis;
	$(APPNAME) CNET=$(INPUT)/cnet.net \
	ONET=$(OUTPUT)/cnet.net \
	LOG=$(OUTPUT)/log.txt \
	CHECKVALID=yes \
       	FROMLIST=$(OUTPUT)/list.lis \
	DEFFILE=$(INPUT)/deffile.def > /dev/null;
	$(RM) $(OUTPUT)/list.lis;
