APPNAME = fits2isis

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/lsb_0034933739_0x53c_sci_1.fit \
		to=$(OUTPUT)/bilTruth.cub organization=bil imagenumber=1 > /dev/null;
	catlab from=$(OUTPUT)/bilTruth.cub \
		to=$(OUTPUT)/bilTruth.pvl > /dev/null;
	$(APPNAME) from=$(INPUT)/lsb_0034933739_0x53c_sci_1.fit \
		to=$(OUTPUT)/bsqTruth.cub organization=bsq imagenumber=1 > /dev/null;
	catlab from=$(OUTPUT)/bsqTruth.cub \
		to=$(OUTPUT)/bsqTruth.pvl > /dev/null;
