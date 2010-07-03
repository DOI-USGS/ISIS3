APPNAME = footprintinit

include $(ISISROOT)/make/isismake.tsts

# cp so I don't destroy the input cube
commands:
	$(CP) $(INPUT)/V30027015RDR.even.cub $(OUTPUT)/V30027015RDR.even.cub;
	$(APPNAME) from=$(OUTPUT)/V30027015RDR.even.cub sinc=65 linc=65 \
	> /dev/null;
	$$ISISROOT/bin/blobdump \
	from=$(OUTPUT)/V30027015RDR.even.cub \
	to=$(OUTPUT)/V30027015RDR.even.txt \
	name=footprint \
	type=Polygon > /dev/null;
	rm $(OUTPUT)/V30027015RDR.even.cub
