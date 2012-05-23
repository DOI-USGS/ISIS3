APPNAME = cnetedit

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) $(INPUT)/*.cub > $(OUTPUT)/list.lis;
	$(CAT) $(INPUT)/measurelist.txt | sed 's#X#$(INPUT)#' \
	  > $(OUTPUT)/measurelist.txt;
	$(APPNAME) CNET=$(INPUT)/cnet.net \
	CUBELIST=$(OUTPUT)/list.lis \
	MEASURELIST=$(OUTPUT)/measurelist.txt \
	ONET=$(OUTPUT)/cnet.net DELETE=no \
	IGNOREALL=yes \
	log=$(OUTPUT)/log.txt > /dev/null;
	$(RM) $(OUTPUT)/list.lis $(OUTPUT)/measurelist.txt;
