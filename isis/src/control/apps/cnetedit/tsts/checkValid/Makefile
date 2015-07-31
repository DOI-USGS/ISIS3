APPNAME = cnetedit

include $(ISISROOT)/make/isismake.tsts

commands:
# normal test
	$(LS) $(INPUT)/*.cub > $(OUTPUT)/list.lis;
	$(APPNAME) CNET=$(INPUT)/cnet.net \
	ONET=$(OUTPUT)/cnet.net \
	LOG=$(OUTPUT)/log.txt \
	CHECKVALID=yes \
       	FROMLIST=$(OUTPUT)/list.lis \
	DEFFILE=$(INPUT)/deffile.def \
	RETAIN_REFERENCE=yes > /dev/null;

# test with IGNOREALL=true
#	$(LS) $(INPUT)/*.cub > $(OUTPUT)/list.lis;
	$(APPNAME) CNET=$(INPUT)/cnet.net \
	ONET=$(OUTPUT)/cnet2.net \
	LOG=$(OUTPUT)/log2.txt \
	CHECKVALID=yes \
	IGNOREALL=true \
       	FROMLIST=$(OUTPUT)/list.lis \
	DEFFILE=$(INPUT)/deffile.def > /dev/null;

# Cleanup
	$(RM) $(OUTPUT)/list.lis;
