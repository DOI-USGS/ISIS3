# run spice init with default parameters only
APPNAME = spiceinit

include $(ISISROOT)/make/isismake.tsts

commands:
	cp $(INPUT)/spiceinitTruth.cub $(OUTPUT)/ > /dev/null;
	$(APPNAME) from=$(OUTPUT)/spiceinitTruth.cub > /dev/null;
	catlab from=$(OUTPUT)/spiceinitTruth.cub to=$(OUTPUT)/labels.pvl > /dev/null;
	$(APPNAME) from=$(OUTPUT)/spiceinitTruth.cub > /dev/null;
	catlab from=$(OUTPUT)/spiceinitTruth.cub to=$(OUTPUT)/labelsTwice.pvl > /dev/null;
	rm $(OUTPUT)/spiceinitTruth.cub;
