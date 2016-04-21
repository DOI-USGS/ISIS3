APPNAME = cnetedit

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) $(INPUT)/*.cub > $(OUTPUT)/list.lis;
	$(APPNAME) CNET=$(INPUT)/cnet.net \
	LOG=$(OUTPUT)/log.txt \
	CUBELIST=$(OUTPUT)/list.lis \
	POINTLIST=$(INPUT)/points.lis \
	DELETE = true \
	PRESERVE = true \
	ONET=$(OUTPUT)/cnet2.net > /dev/null;
	$(RM) $(OUTPUT)/list.lis;
