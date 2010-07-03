APPNAME = photomet

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/lub2303j.191.lev1.cub \
	to=$(OUTPUT)/lub2303j.191.lev1.phot.cub \
	phopar=$(INPUT)/clem.pvl \
	maxincidence=5.0 > /dev/null;
