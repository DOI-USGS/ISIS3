APPNAME = spiceinit

include $(ISISROOT)/make/isismake.tsts

commands:
	cp $(INPUT)/spiceinitTruth.cub $(OUTPUT)/ > /dev/null;
	$(APPNAME) from=$(OUTPUT)/spiceinitTruth.cub > /dev/null;
	catlab from=$(OUTPUT)/spiceinitTruth.cub > $(OUTPUT)/labels.pvl;
	$(APPNAME) from=$(OUTPUT)/spiceinitTruth.cub > /dev/null;
	catlab from=$(OUTPUT)/spiceinitTruth.cub > $(OUTPUT)/labelsTwice.pvl;
	rm $(OUTPUT)/spiceinitTruth.cub;
