APPNAME = cnetedit

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) $(INPUT)/*.cub > $(INPUT)/list.lis;
	$(APPNAME) CNET=$(INPUT)/cnet.net \
	CUBELIST=$(INPUT)/list.lis \
	ONET=$(OUTPUT)/cnet.pvl > /dev/null;
	$(RM) $(INPUT)/list.lis;
