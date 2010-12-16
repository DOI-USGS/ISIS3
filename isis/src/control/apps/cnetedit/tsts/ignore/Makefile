APPNAME = cnetedit

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) $(INPUT)/*.cub > $(OUTPUT)/list.lis;
	$(APPNAME) CNET=$(INPUT)/cnet.net \
	CUBELIST=$(OUTPUT)/list.lis \
	ONET=$(OUTPUT)/cnet.net > /dev/null;
	$(RM) $(OUTPUT)/list.lis;
