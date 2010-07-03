APPNAME = gllssi2isis

7500r.txt.IGNORELINES = Bytes StartByte ByteOrder

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) FROM=$(INPUT)/7500r.lbl \
	TO=$(OUTPUT)/7500r.cub > /dev/null;
	catlab FROM=$(OUTPUT)/7500r.cub > $(OUTPUT)/7500r.txt;
