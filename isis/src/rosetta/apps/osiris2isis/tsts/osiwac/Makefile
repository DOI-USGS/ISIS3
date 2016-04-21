APPNAME = osiris2isis

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/w20080903t181608542id30f18.img \
	  to=$(OUTPUT)/w20080903t181608542id30f18.cub > /dev/null

	catlab from=$(OUTPUT)/w20080903t181608542id30f18.cub \
	to=$(OUTPUT)/w20080903t181608542id30f18.pvl > /dev/null