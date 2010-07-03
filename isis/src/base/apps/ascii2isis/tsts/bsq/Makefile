APPNAME = ascii2isis

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/ascii2isis_input.txt \
	to=$(OUTPUT)/ascii2isisBsqTruth.cub \
	order=bsq \
	lines=4 \
	samples=2 \
	bands=2	\
	skip=35  > /dev/null;
