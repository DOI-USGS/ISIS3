APPNAME = cnetedit

include $(ISISROOT)/make/isismake.tsts

commands:

# general test
	$(CAT) $(INPUT)/bad_measures.lis | $(SED) "s#,#,$(INPUT)/#" \
		> $(OUTPUT)/list.lis;

	$(APPNAME) CNET=$(INPUT)/cnet.net \
	LOG=$(OUTPUT)/log.txt \
	MEASURELIST=$(OUTPUT)/list.lis \
	DELETE=no \
	ONET=$(OUTPUT)/cnet.net > /dev/null;

# test with IGNOREALL = true, DELETE=no measure that is a reference for a non-ignored point
	$(CAT) $(INPUT)/bad_measures2.lis | $(SED) "s#,#,$(INPUT)/#" \
		> $(OUTPUT)/list2.lis;

	$(APPNAME) CNET=$(INPUT)/cnet.net \
	LOG=$(OUTPUT)/log2.txt \
	MEASURELIST=$(OUTPUT)/list2.lis \
	IGNOREALL=true \
	DELETE=no \
	ONET=$(OUTPUT)/cnet2.net > /dev/null;

# test with DELETE=yes
	$(CAT) $(INPUT)/bad_measures.lis | $(SED) "s#,#,$(INPUT)/#" \
		> $(OUTPUT)/list.lis;

	$(APPNAME) CNET=$(INPUT)/cnet.net \
	LOG=$(OUTPUT)/log3.txt \
	MEASURELIST=$(OUTPUT)/list.lis \
	ONET=$(OUTPUT)/cnet3.net > /dev/null;

# Cleanup
	$(RM) $(OUTPUT)/list.lis;
	$(RM) $(OUTPUT)/list2.lis;
