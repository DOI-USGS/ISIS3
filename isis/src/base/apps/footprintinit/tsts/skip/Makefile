APPNAME = footprintinit

include $(ISISROOT)/make/isismake.tsts

# cp so I don't destroy the input cube
commands:
	$(CP) $(INPUT)/EN0108821505M.cal.cub $(OUTPUT)/EN0108821505M.cal.cub;
	$(APPNAME) from=$(OUTPUT)/EN0108821505M.cal.cub \
	sinc=3 linc=3 > /dev/null;
	$$ISISROOT/bin/blobdump \
	from=$(OUTPUT)/EN0108821505M.cal.cub \
	to=$(OUTPUT)/EN0108821505M.txt \
	name=footprint \
	type=Polygon > /dev/null;
	rm $(OUTPUT)/EN0108821505M.cal.cub;
