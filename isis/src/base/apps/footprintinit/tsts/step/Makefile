APPNAME = footprintinit

include $(ISISROOT)/make/isismake.tsts

# cp so I don't destroy the input cube
commands:
	$(CP) $(INPUT)/PSP_001365_2720_RED4_0.cub $(OUTPUT)/long.cub;
	$(APPNAME) from=$(OUTPUT)/long.cub linc=1500 > /dev/null;
	$$ISISROOT/bin/blobdump from=$(OUTPUT)/long.cub \
	to=$(OUTPUT)/long.txt name=footprint type=Polygon > /dev/null;
	rm $(OUTPUT)/long.cub
	$(CP) $(INPUT)/PSP_001365_2720_RED4_0.cub $(OUTPUT)/wide.cub;
	$(APPNAME) from=$(OUTPUT)/wide.cub sinc=1500 >/dev/null;
	$$ISISROOT/bin/blobdump from=$(OUTPUT)/wide.cub \
	to=$(OUTPUT)/wide.txt name=footprint type=Polygon > /dev/null;
	rm $(OUTPUT)/wide.cub
