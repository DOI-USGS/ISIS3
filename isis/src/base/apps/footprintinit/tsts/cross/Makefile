APPNAME = footprintinit

include $(ISISROOT)/make/isismake.tsts

# cp so I don't destroy the input cube
commands:
	$(CP) $(INPUT)/f174s47.cub $(OUTPUT)/f174s47.cub;
	$(APPNAME) from=$(OUTPUT)/f174s47.cub \
	sinc=100 linc=100 > /dev/null;
	$$ISISROOT/bin/blobdump \
	from=$(OUTPUT)/f174s47.cub \
	to=$(OUTPUT)/f174s47.txt \
	name=footprint \
	type=Polygon > /dev/null;
	rm $(OUTPUT)/f174s47.cub
