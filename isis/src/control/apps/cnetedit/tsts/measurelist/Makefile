APPNAME = cnetedit

include $(ISISROOT)/make/isismake.tsts

commands:
	$(CAT) $(INPUT)/bad_measures.lis | $(SED) "s#,#,$(INPUT)/#" \
		> $(OUTPUT)/list.lis;

	$(APPNAME) CNET=$(INPUT)/cnet.net \
	LOG=$(OUTPUT)/log.txt \
	MEASURELIST=$(OUTPUT)/list.lis \
	DELETE=no \
	ONET=$(OUTPUT)/cnet.net > /dev/null;

	$(RM) $(OUTPUT)/list.lis;
