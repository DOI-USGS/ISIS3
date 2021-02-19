# Test for output errors in mat2cnet.  No control net will be output, instead
# we will have a text file containing the errors.
.IGNORE:

APPNAME = mat2cnet

include $(ISISROOT)/make/isismake.tsts

commands:
	ls $(INPUT)/Isis3_cube*.cub >& $(OUTPUT)/cub3.lis;
	$(APPNAME) \
	list2=$(INPUT)/cub2.lis \
	match=$(INPUT)/match.mat \
	list3=$(OUTPUT)/cub3.lis \
	onet=$(OUTPUT)/out.net \
	description="Isis2 list has different number of lines from Isis list" \
	networkid=TestNet01 \
	target=Titan 2> $(OUTPUT)/error1.txt > /dev/null;
	ls $(INPUT)/Isis2_cube*.cub >& $(OUTPUT)/cub2.lis;
	$(APPNAME) list2=$(OUTPUT)/cub2.lis \
	match=$(INPUT)/match_noMatchptKeywd.mat \
	list3=$(OUTPUT)/cub3.lis \
	onet=$(OUTPUT)/out.net \
	description="Invalid MATCH file header format - missing number of measurements" \
	networkid=TestNet02 \
	target=Titan 2>> $(OUTPUT)/error1.txt > /dev/null;
	$(APPNAME) list2=$(OUTPUT)/cub2.lis \
	match=$(INPUT)/match_noColumnLabels.mat \
	list3=$(OUTPUT)/cub3.lis \
	onet=$(OUTPUT)/out.net \
	description="Invalid MATCH file header format - second line not labels" \
	networkid=TestNet03 \
	target=Titan 2>> $(OUTPUT)/error1.txt > /dev/null;
	$(APPNAME) list2=$(OUTPUT)/cub2.lis \
	match=$(INPUT)/match_invalidKeyword.mat \
	list3=$(OUTPUT)/cub3.lis \
	onet=$(OUTPUT)/out.net \
	description="Invalid MATCH file - Matchpoint total less than number of measurements found" \
	networkid=TestNet04 \
	target=Titan 2>> $(OUTPUT)/error1.txt > /dev/null;
	$(APPNAME) list2=$(OUTPUT)/cub2.lis \
	match=$(INPUT)/match_invalidMeasType.mat \
	list3=$(OUTPUT)/cub3.lis \
	onet=$(OUTPUT)/out.net \
	description="Invalid MATCH file - unknown measurement type" \
	networkid=TestNet05 \
	target=Titan 2>> $(OUTPUT)/error1.txt > /dev/null;
	$(APPNAME) list2=$(OUTPUT)/cub2.lis \
	match=$(INPUT)/match_repeatedPointID.mat \
	list3=$(OUTPUT)/cub3.lis \
	onet=$(OUTPUT)/out.net \
	description="Invalid MATCH file - repeated pointID/fsc combination" \
	networkid=TestNet06 \
	target=Titan 2>> $(OUTPUT)/error1.txt > /dev/null;
	$(APPNAME) list2=$(OUTPUT)/cub2.lis \
	match=$(INPUT)/match_invalidLineSamp.mat \
	list3=$(OUTPUT)/cub3.lis \
	onet=$(OUTPUT)/out.net \
	description="Invalid MATCH file - sample/line/diameter not a double" \
	networkid=TestNet07 \
	target=Titan 2>> $(OUTPUT)/error1.txt > /dev/null;
	$(APPNAME) list2=$(OUTPUT)/cub2.lis \
	match=$(INPUT)/match.mat \
	inputppp=true \
	ppp=$(INPUT)/rand_invalidLineCount.ppp \
	list3=$(OUTPUT)/cub3.lis \
	onet=$(OUTPUT)/out.net \
	description="Invalid RAND file - line count less than points found" \
	networkid=TestNet09 \
	target=Titan 2>> $(OUTPUT)/error1.txt > /dev/null;
	$(APPNAME) list2=$(OUTPUT)/cub2.lis \
	match=$(INPUT)/match.mat \
	inputppp=true \
	ppp=$(INPUT)/rand_invalidLatLonRad.ppp \
	list3=$(OUTPUT)/cub3.lis \
	onet=$(OUTPUT)/out.net \
	description="Invalid RAND file - lat/lon/rad not a double" \
	networkid=TestNet10 \
	target=Titan 2>> $(OUTPUT)/error1.txt > /dev/null;
	$(SED) 's+/.*/input/+input/+' $(OUTPUT)/error1.txt > $(OUTPUT)/error.txt;
	rm -f $(OUTPUT)/error1.txt;
	rm -f $(OUTPUT)/out.net;
	rm -f $(OUTPUT)/cub2.lis;
	rm -f $(OUTPUT)/cub3.lis;
