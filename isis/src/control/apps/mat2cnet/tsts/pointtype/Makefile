APPNAME = mat2cnet

include $(ISISROOT)/make/isismake.tsts

commands:
	ls $(INPUT)/Isis2_cube*.cub >& $(OUTPUT)/cub2.lis;
	ls $(INPUT)/Isis3_cube*.cub >& $(OUTPUT)/cub3.lis;
	$(APPNAME) list2=$(OUTPUT)/cub2.lis \
	match=$(INPUT)/pts.mat \
    inputppp=true \
	ppp=$(INPUT)/rand.ppp \
	list3=$(OUTPUT)/cub3.lis \
	onet=$(OUTPUT)/mat2cnetTruth3.net \
	pointtype=Free \
	description="PointTypeTie Test Network" \
	networkid=TestNet03 \
	target=Titan > /dev/null;
	mv randOnlyPoints.log $(OUTPUT)/randOnlyPoints.txt;
	rm -f $(OUTPUT)/cub2.lis;
	rm -f $(OUTPUT)/cub3.lis;
