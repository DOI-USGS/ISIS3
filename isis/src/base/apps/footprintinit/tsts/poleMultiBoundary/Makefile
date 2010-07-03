APPNAME = footprintinit

include $(ISISROOT)/make/isismake.tsts

# cp so I don't destroy the input cube
commands:
	$(CP) $(INPUT)/e1202081.cub $(OUTPUT)/e1202081.cub;
	$(APPNAME) from=$(OUTPUT)/e1202081.cub sinc=17 linc=17 \
	> /dev/null;
	$$ISISROOT/bin/blobdump \
	from=$(OUTPUT)/e1202081.cub \
	to=$(OUTPUT)/e1202081.txt \
	name=footprint \
	type=Polygon > /dev/null;
	rm $(OUTPUT)/e1202081.cub
