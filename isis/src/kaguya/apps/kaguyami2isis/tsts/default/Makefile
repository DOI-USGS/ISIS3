APPNAME = kaguyami2isis

#NOPREFERENCES=
include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/vis.img \
	  to=$(OUTPUT)/vis.cub > /dev/null
	$(APPNAME) from=$(INPUT)/nir.img \
	  to=$(OUTPUT)/nir.cub > /dev/null
	$(APPNAME) from=$(INPUT)/MVA_2B2_01_01229N603E1343 \
	  to=$(OUTPUT)/MVA_2B2_01_01229N603E1343.cub > /dev/null
