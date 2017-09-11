APPNAME = vik2isis

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/f319b18.imq to=$(OUTPUT)/vik2isis-imqTruth.cub > /dev/null;
	
	catlab from=$(OUTPUT)/vik2isis-imqTruth.cub to=$(OUTPUT)/vik2isis-imqTruth.pvl > /dev/null;

