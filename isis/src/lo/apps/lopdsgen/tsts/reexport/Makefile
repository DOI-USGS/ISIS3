APPNAME = lopdsgen

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/3133_h1.cub \
	to=$(OUTPUT)/3133_h1.pds.img \
	> /dev/null;
	catlab from=$(OUTPUT)/3133_h1.pds.img \
	to=$(OUTPUT)/3133_h1_1.pvl \
	> /dev/null;
	lo2isis from=$(OUTPUT)/3133_h1.pds.img \
	to=$(OUTPUT)/3133_h1_1.cub \
	> /dev/null;
	cubediff from=$(INPUT)/3133_h1.cub \
	from2=$(OUTPUT)/3133_h1_1.cub \
	to=$(OUTPUT)/cube_difference.txt \
	> /dev/null;
	rm $(OUTPUT)/3133_h1.pds.img;
	rm $(OUTPUT)/3133_h1_1.cub;
