APPNAME = cnetstats

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) -1 $(INPUT)/*.cub > $(OUTPUT)/cube.lis;
	$(APPNAME) fromlist=$(OUTPUT)/cube.lis \
	  cnet=$(INPUT)/cnet.net \
	  filter=no \
	  create_image_stats=yes \
	  image_stats_file=$(OUTPUT)/imagestats.txt \
	  create_point_stats=no > /dev/null;
	  $(RM) $(OUTPUT)/cube.lis;
