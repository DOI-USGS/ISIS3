APPNAME = findrx

include $(ISISROOT)/make/isismake.tsts

commands:
	cp $(INPUT)/f387a06.cub $(OUTPUT)/f387a06.cub;
	$(APPNAME) from=$(OUTPUT)/f387a06.cub > /dev/null;
	catlab from=$(OUTPUT)/f387a06.cub > $(OUTPUT)/findrxTruth.pvl 2>&1;
