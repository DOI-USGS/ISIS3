# Test for mat2cnet using PPP file with parameters
#   POINTTYPE=Free (sets points in PPP file to PointType=Free in output cnet)
#   POINTLOCK=false (sets points in PPP file to EditLock=false in output cnet)
#   LOG=randppp.txt file (includes points in PPP that are excluded from the
#       output cnet since they are not in the match file, i.e. have no measures)
APPNAME = mat2cnet

include $(ISISROOT)/make/isismake.tsts

commands:
	ls $(INPUT)/Isis2_cube*.cub >& $(OUTPUT)/cub2.lis;
	ls $(INPUT)/Isis3_cube*.cub >& $(OUTPUT)/cub3.lis;
	$(APPNAME) list2=$(OUTPUT)/cub2.lis \
	match=$(INPUT)/matchpointfile.mat \
    inputppp=true \
	ppp=$(INPUT)/rand.ppp \
	pointtype=Free \
	pointlock=false \
    log=$(OUTPUT)/randppp.txt \
	list3=$(OUTPUT)/cub3.lis \
	onet=$(OUTPUT)/mat2cnetTruth3.net \
	description="Rand Test Network - Free Point Type, Unlocked Points" \
	networkid=TestNet03 \
	target=Titan > /dev/null;
	rm -f $(OUTPUT)/cub2.lis;
	rm -f $(OUTPUT)/cub3.lis;
