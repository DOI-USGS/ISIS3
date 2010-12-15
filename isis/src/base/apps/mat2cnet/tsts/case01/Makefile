APPNAME = mat2cnet

include $(ISISROOT)/make/isismake.tsts

commands:
	ls $(INPUT)/Isis2_cube*.cub >& $(OUTPUT)/cub2.lis;
	ls $(INPUT)/Isis3_cube*.cub >& $(OUTPUT)/cub3.lis;
	$(APPNAME) list2=$(OUTPUT)/cub2.lis \
	match=$(INPUT)/pts.mat \
	list3=$(OUTPUT)/cub3.lis \
	cnet=$(OUTPUT)/mat2cnetTruth1.net \
	description="Default Test Network" \
	networkid=TestNet01 \
	target=Titan > /dev/null;
	rm -f $(OUTPUT)/cub2.lis;
	rm -f $(OUTPUT)/cub3.lis;
