APPNAME = cnetstats

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) -1 $(INPUT)/*.cub > $(OUTPUT)/cube.lis;
	if [ `$(APPNAME) \
	  fromlist=$(OUTPUT)/cube.lis \
	  cnet=$(INPUT)/cnet.net \
	  filter=no \
	  create_image_stats=yes \
	  image_stats_file=/image.txt \
	  create_point_stats=no \
	  > /dev/null 2> $(OUTPUT)/imageError.txt` ]; then \
	  true; \
	fi;  
	if [ `$(APPNAME) \
	  fromlist=$(OUTPUT)/cube.lis \
	  cnet=$(INPUT)/cnet.net \
	  filter=no \
	  create_image_stats=no \
	  create_point_stats=yes \
	  point_stats_file=/points.txt \
	  > /dev/null 2> $(OUTPUT)/pointError.txt` ]; then \
	  true; \
	fi;  
	$(RM) $(OUTPUT)/cube.lis



