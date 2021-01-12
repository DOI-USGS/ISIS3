APPNAME = isis2pds

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/ab102401.cub to=$(OUTPUT)/ab102401.img > /dev/null;
	catlab from=$(OUTPUT)/ab102401.img to=$(OUTPUT)/labels.pvl > /dev/null;
	pds2isis from=$(OUTPUT)/ab102401.img to=$(OUTPUT)/ab102401.cub > /dev/null;
	dd if=$(OUTPUT)/ab102401.img \
	bs=`getkey from=$(OUTPUT)/ab102401.img keyword=^IMAGE` \
	count=1 >& $(OUTPUT)/TEMP.txt > $(OUTPUT)/labelsEOL.txt; 
	$(RM) $(OUTPUT)/ab102401.img > /dev/null;
	$(APPNAME) from=$(INPUT)/ab102401.cub labtype=fixed to=$(OUTPUT)/ab102401Fixed.img > /dev/null;
	catlab from=$(OUTPUT)/ab102401Fixed.img to=$(OUTPUT)/labelsFixed.pvl > /dev/null;
	pds2isis from=$(OUTPUT)/ab102401Fixed.img to=$(OUTPUT)/ab102401Fixed.cub > /dev/null;
	dd if=$(OUTPUT)/ab102401Fixed.img \
	bs=`getkey from=$(OUTPUT)/ab102401Fixed.img keyword=^IMAGE` \
	count=1 >& $(OUTPUT)/TEMP.txt > $(OUTPUT)/labelsFixedEOL.txt; 
	$(RM) $(OUTPUT)/ab102401Fixed.img > /dev/null;
	$(RM) $(OUTPUT)/TEMP.txt > /dev/null;
