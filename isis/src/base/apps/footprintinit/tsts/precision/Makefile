APPNAME = footprintinit

include $(ISISROOT)/make/isismake.tsts

# cp so I don't destroy the input cube
commands:
	$(CP) $(INPUT)/N1466508085_1.cub $(OUTPUT)/N1466508085_1.cub;
	$(APPNAME) from=$(OUTPUT)/N1466508085_1.cub \
	testxy=true increaseprecision=true > /dev/null;
	$$ISISROOT/bin/blobdump \
	from=$(OUTPUT)/N1466508085_1.cub \
	to=$(OUTPUT)/N1466508085_1.txt \
	name=footprint \
	type=Polygon > /dev/null;
	rm $(OUTPUT)/N1466508085_1.cub
