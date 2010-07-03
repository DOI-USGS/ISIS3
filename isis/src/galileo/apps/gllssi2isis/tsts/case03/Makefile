APPNAME = gllssi2isis

labels.txt.IGNORELINES = Bytes StartByte ByteOrder

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) FROM=$(INPUT)/3700R.LBL \
	TO=$(OUTPUT)/3700R.cub > /dev/null;
	catlab FROM=$(OUTPUT)/3700R.cub > $(OUTPUT)/3700R.txt;
