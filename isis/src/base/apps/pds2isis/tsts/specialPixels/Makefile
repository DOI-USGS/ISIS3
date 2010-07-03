APPNAME = pds2isis

labels.txt.IGNORELINES = Bytes StartByte ByteOrder

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/ab102401.img \
	to=$(OUTPUT)/pds2isisTruth.cub+32bit \
	setnullrange=yes nullmin=15.0 nullmax=45.0 \
	sethrsrange=yes hrsmin=220.0 hrsmax=250.0 \
	sethisrange=yes hismin=190.0 hismax=219.0 \
	setlrsrange=yes lrsmin=96.0 lrsmax=125.0 \
	setlisrange=yes lismin=65.0 lismax=95.0 \
	> /dev/null;
	catlab from=$(OUTPUT)/pds2isisTruth.cub >& $(OUTPUT)/labels.txt;
