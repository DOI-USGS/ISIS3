APPNAME = isis2std

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) red=$(INPUT)/PSP_001892_1715_COLOR.cub+1 \
	  green=$(INPUT)/PSP_001892_1715_COLOR.cub+2 \
	  blue=$(INPUT)/PSP_001892_1715_COLOR.cub+3 \
	  to=$(OUTPUT)/PSP_001892_1715_COLOR.jp2 \
	  mode=rgb \
	  format=jp2 \
	  stretch=linear > /dev/null;
	std2isis FROM=$(OUTPUT)/PSP_001892_1715_COLOR.jp2 \
	  TO=$(OUTPUT)/PSP_001892_1715_COLOR.cub > /dev/null;
	rm -rf $(OUTPUT)/PSP_001892_1715_COLOR.jp2;
