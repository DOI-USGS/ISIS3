APPNAME = ascii2isis

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/ascii2isis_input.txt \
	to=$(OUTPUT)/ascii2isisBilTruth.cub \
	order=bil \
	lines=4 \
	samples=2 \
	bands=2 > /dev/null;
