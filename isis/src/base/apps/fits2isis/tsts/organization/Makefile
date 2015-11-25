APPNAME = fits2isis

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/lsb_0034933739_0x53c_sci_1.fit \
	to=$(OUTPUT)/bilTruth.cub organization=bil imagenumber=1\
	> /dev/null;
	$(APPNAME) from=$(INPUT)/lsb_0034933739_0x53c_sci_1.fit \
	to=$(OUTPUT)/bsqTruth.cub organization=bsq imagenumber=1 \
	> /dev/null;
