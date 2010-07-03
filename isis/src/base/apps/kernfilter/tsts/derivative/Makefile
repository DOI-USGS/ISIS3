APPNAME = kernfilter

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/isisTruth.cub to=$(OUTPUT)/truth.cub \
	kernel=$(INPUT)/derivativeNWtoSE2x2.txt> /dev/null;
	
