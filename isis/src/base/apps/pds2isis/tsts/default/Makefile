APPNAME = pds2isis

labels.txt.IGNORELINES = Bytes StartByte ByteOrder

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/ab102401.img \
	  to=$(OUTPUT)/pds2isisTruth.cub > /dev/null;
	  catlab from=$(OUTPUT)/pds2isisTruth.cub >& $(OUTPUT)/labels.txt;
