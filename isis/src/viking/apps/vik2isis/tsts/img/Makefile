APPNAME = vik2isis

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/f319b18.imq to=$(OUTPUT)/vik2isis-imgTruth.cub > /dev/null;
	
	catlab from=$(OUTPUT)/vik2isis-imgTruth.cub to=$(OUTPUT)/vik2isis-imgTruth.pvl > /dev/null;
