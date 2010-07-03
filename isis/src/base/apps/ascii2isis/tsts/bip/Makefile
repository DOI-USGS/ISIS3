APPNAME = ascii2isis

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/ascii2isis_input.txt \
	to=$(OUTPUT)/ascii2isisBipTruth.cub \
	order=bip \
	lines=4 \
	samples=2 \
	bands=2	> /dev/null;
