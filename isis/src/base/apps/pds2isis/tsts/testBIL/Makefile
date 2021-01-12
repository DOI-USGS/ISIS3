APPNAME = pds2isis

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/BILtestData.LBL \
	to=$(OUTPUT)/testBILtruth.cub+32bit > /dev/null;
	catlab from=$(OUTPUT)/testBILtruth.cub to=$(OUTPUT)/testBILtruth.pvl > /dev/null;
   
