APPNAME = nirs2isis

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/2392975548_lvl3_0.lbl \
	to=$(OUTPUT)/2392975548_lvl3_0.refl.cub \
	tostddev=$(OUTPUT)/2392975548_lvl3_0.stdev.cub \
	> /dev/null;
	catlab from=$(OUTPUT)/2392975548_lvl3_0.refl.cub \
	to=$(OUTPUT)/2392975548_lvl3_0.refl.pvl \
	> /dev/null;
	catlab from=$(OUTPUT)/2392975548_lvl3_0.stdev.cub \
	to=$(OUTPUT)/2392975548_lvl3_0.stdev.pvl \
	> /dev/null;
	catoriglab from=$(OUTPUT)/2392975548_lvl3_0.refl.cub \
	to=$(OUTPUT)/2392975548_lvl3_0.refl.orig.pvl \
	> /dev/null;
	catoriglab from=$(OUTPUT)/2392975548_lvl3_0.stdev.cub \
	to=$(OUTPUT)/2392975548_lvl3_0.stdev.orig.pvl \
	> /dev/null;
