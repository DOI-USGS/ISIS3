APPNAME = cnetstats

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) -1 $(INPUT)/*.cub > $(OUTPUT)/cube.lis;
	$(APPNAME) fromlist=$(OUTPUT)/cube.lis \
	  cnet=$(INPUT)/cnet.net \
	  filter=no \
	  create_image_stats=no \
	  create_point_stats=yes \
	  point_stats_file=$(OUTPUT)/truth.txt > /dev/null;
	  $(RM) $(OUTPUT)/cube.lis;
