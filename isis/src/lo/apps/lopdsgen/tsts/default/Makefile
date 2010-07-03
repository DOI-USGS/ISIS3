APPNAME = lopdsgen

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/3133_h1_isis3.cub \
	to=$(OUTPUT)/3133_h1.pds.img \
	> /dev/null;
	catlab from=$(OUTPUT)/3133_h1.pds.img \
	to=$(OUTPUT)/3133_h1.pvl \
	> /dev/null;
	rm $(OUTPUT)/3133_h1.pds.img;
