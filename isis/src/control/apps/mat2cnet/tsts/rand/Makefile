APPNAME = mat2cnet

include $(ISISROOT)/make/isismake.tsts

commands:
	ls $(INPUT)/Isis2_cube*.cub >& $(OUTPUT)/cub2.lis;
	ls $(INPUT)/Isis3_cube*.cub >& $(OUTPUT)/cub3.lis;
	$(APPNAME) list2=$(OUTPUT)/cub2.lis \
	match=$(INPUT)/match.mat \
	inputppp=true \
	ppp=$(INPUT)/rand.ppp \
	list3=$(OUTPUT)/cub3.lis \
	onet=$(OUTPUT)/mat2cnetTruth2.net \
	description="Rand Test Network - Default Ground Point Type" \
	networkid=TestNet02 \
	target=Titan \
    log=$(OUTPUT)/randppp.txt > /dev/null;
	rm -f $(OUTPUT)/cub2.lis;
	rm -f $(OUTPUT)/cub3.lis;
