APPNAME = map2map

include $(ISISROOT)/make/isismake.tsts
# testing for when the Scale keyword is missing from the input cube and the map.
commands: 
	# scale keyword missing
	$(APPNAME) FROM=$(INPUT)/dullesSmall.cub \
	MAP=$(INPUT)/noScaleSmall.pvl \
	TO=$(OUTPUT)/dullesNoScaleSmall.cub \
	MATCHMAP=yes > /dev/null;

	#scale keyword is defined
	$(APPNAME) FROM=$(INPUT)/dullesSmallYesScale.cub \
	MAP=$(INPUT)/yesScaleSmall.pvl \
	TO=$(OUTPUT)/dullesYesScaleSmall.cub \
	MATCHMAP=yes > /dev/null;