APPNAME = mocuncompress

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) $(INPUT)/m0402852.imq $(OUTPUT)/temp.imq > /dev/null;
	pds2isis from=$(OUTPUT)/temp.imq to=$(OUTPUT)/mocuncompressTruth.cub > /dev/null;
