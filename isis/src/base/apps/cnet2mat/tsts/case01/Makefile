APPNAME = cnet2mat

include $(ISISROOT)/make/isismake.tsts

commands:
	ls $(INPUT)/Isis2_cube*.cub >& $(OUTPUT)/cub2.lis;
	ls $(INPUT)/Isis3_cube*.cub >& $(OUTPUT)/cub3.lis;
	$(APPNAME) list3=$(OUTPUT)/cub3.lis \
	control=$(INPUT)/testNet.ctl \
	list2=$(OUTPUT)/cub2.lis \
	match=$(OUTPUT)/cnet2matTruth1.txt > /dev/null;
	rm -f $(OUTPUT)/cub2.lis;
	rm -f $(OUTPUT)/cub3.lis;
