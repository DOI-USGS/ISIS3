APPNAME = footprintinit

include $(ISISROOT)/make/isismake.tsts

# cp so I don't destroy the input cube
commands:
	$(CP) $(INPUT)/CM_1504753137_1.ir.cub \
	$(OUTPUT)/CM_1504753137_1.ir.cub;
	$(APPNAME) from=$(OUTPUT)/CM_1504753137_1.ir.cub \
	sinc=5 linc=5 > /dev/null;
	$$ISISROOT/bin/blobdump \
	from=$(OUTPUT)/CM_1504753137_1.ir.cub \
	to=$(OUTPUT)/CM_1504753137_1.txt \
	name=footprint \
	type=Polygon > /dev/null;
	$(CP) $(INPUT)/CM_1504753137_1.ir.cub \
	$(OUTPUT)/CM_1504753137_1.2.ir.cub;
	$(APPNAME) from=$(OUTPUT)/CM_1504753137_1.2.ir.cub \
	sinc=5 linc=5 maxemission=50.0 maxincidence=50.0 > /dev/null;
	$$ISISROOT/bin/blobdump \
	from=$(OUTPUT)/CM_1504753137_1.2.ir.cub \
	to=$(OUTPUT)/CM_1504753137_1.2.txt \
	name=footprint \
	type=Polygon > /dev/null;
	rm $(OUTPUT)/CM_1504753137_1.ir.cub
	rm $(OUTPUT)/CM_1504753137_1.2.ir.cub
