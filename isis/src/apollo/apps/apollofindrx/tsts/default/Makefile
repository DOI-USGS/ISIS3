APPNAME = apollofindrx

include $(ISISROOT)/make/isismake.tsts

commands:
	cp $(INPUT)/AS15-M-1450.cub $(OUTPUT)/AS15-M-1450.cub > /dev/null; 
	apollofindrx from=$(OUTPUT)/AS15-M-1450.cub > /dev/null;
	catlab from=$(OUTPUT)/AS15-M-1450.cub \
	to=$(OUTPUT)/AS15-M-1450.pvl > /dev/null; 
	cathist from=$(OUTPUT)/AS15-M-1450.cub \
	to=$(OUTPUT)/histTruth.pvl > /dev/null;
