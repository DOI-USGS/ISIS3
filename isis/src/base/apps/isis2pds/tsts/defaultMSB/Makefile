APPNAME = isis2pds

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/ab102401.cub to=$(OUTPUT)/ab102401.img \
	endian=msb > /dev/null;
	catlab from=$(OUTPUT)/ab102401.img to=$(OUTPUT)/labels.pvl > /dev/null;
	pds2isis from=$(OUTPUT)/ab102401.img to=$(OUTPUT)/ab102401.cub > /dev/null;
	$(RM) $(OUTPUT)/ab102401.img > /dev/null;
	$(APPNAME) from=$(INPUT)/ab102401.cub to=$(OUTPUT)/ab102401Fixed.img \
	labtype=fixed endian=msb > /dev/null;
	catlab from=$(OUTPUT)/ab102401Fixed.img to=$(OUTPUT)/labelsFixed.pvl > /dev/null;
	pds2isis from=$(OUTPUT)/ab102401Fixed.img to=$(OUTPUT)/ab102401Fixed.cub > /dev/null;
	$(RM) $(OUTPUT)/ab102401Fixed.img > /dev/null;
