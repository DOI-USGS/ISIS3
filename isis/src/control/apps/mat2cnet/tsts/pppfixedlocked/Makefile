# Test for mat2cnet using PPP file with default parameters POINTTYPE=Fixed and 
# POINTLOCK=true.  Then, default LOG file is moved to OUTPUT directory
APPNAME = mat2cnet

include $(ISISROOT)/make/isismake.tsts

commands:
	ls $(INPUT)/Isis2_cube*.cub >& $(OUTPUT)/cub2.lis;
	ls $(INPUT)/Isis3_cube*.cub >& $(OUTPUT)/cub3.lis;
	$(APPNAME) list2=$(OUTPUT)/cub2.lis \
	match=$(INPUT)/matchpointfile.mat \
	inputppp=true \
	ppp=$(INPUT)/rand.ppp \
	list3=$(OUTPUT)/cub3.lis \
	onet=$(OUTPUT)/mat2cnetTruth2.net \
	description="Rand Test Network - Default Fixed Point Type, Locked Points" \
	networkid=TestNet02 \
	target=Titan > /dev/null;
	mv pppOnlyPoints.log $(OUTPUT)/pppOnlyPoints.txt;
	rm -f $(OUTPUT)/cub2.lis;
	rm -f $(OUTPUT)/cub3.lis;
