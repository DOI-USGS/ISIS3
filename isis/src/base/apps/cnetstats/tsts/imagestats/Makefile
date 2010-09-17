APPNAME = cnetstats

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) -1 $(INPUT)/*.cub > $(OUTPUT)/cube.lis;
	$(APPNAME) fromlist=$(OUTPUT)/cube.lis \
	cnet=$(INPUT)/cnet.net \
	filter=no \
	create_image_stats=yes \
	image_stats_file=$(OUTPUT)/out.txt \
	create_point_stats=no > /dev/null;
	$(SED) 's/.*\/input\///' $(OUTPUT)/out.txt > $(OUTPUT)/truth.txt;
	$(RM) $(OUTPUT)/cube.lis $(OUTPUT)/out.txt;
