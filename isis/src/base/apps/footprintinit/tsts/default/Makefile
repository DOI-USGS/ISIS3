APPNAME = footprintinit

include $(ISISROOT)/make/isismake.tsts

# cp so I don't destroy the input cube
commands:
	$(CP) $(INPUT)/ab102401.cub $(OUTPUT)/ab102401.cub;
	$(APPNAME) from=$(OUTPUT)/ab102401.cub sinc=1 linc=1 > /dev/null;
	$$ISISROOT/bin/blobdump \
	from=$(OUTPUT)/ab102401.cub \
	to=$(OUTPUT)/ab102401.txt \
	name=footprint \
	type=Polygon > /dev/null;
	rm $(OUTPUT)/ab102401.cub
