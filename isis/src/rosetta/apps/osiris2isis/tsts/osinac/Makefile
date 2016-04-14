APPNAME = osiris2isis

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/n20081004t160210690id30f22.img \
	to=$(OUTPUT)/n20081004t160210690id30f22.cub > /dev/null

	catlab from=$(OUTPUT)/n20081004t160210690id30f22.cub \
	to=$(OUTPUT)/n20081004t160210690id30f22.pvl > /dev/null
