APPNAME = gllssi2isis

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) FROM=$(INPUT)/4911s.lbl \
	TO=$(OUTPUT)/4911s.cub > /dev/null;
	catlab FROM=$(OUTPUT)/4911s.cub to=$(OUTPUT)/4911s.txt > /dev/null;
