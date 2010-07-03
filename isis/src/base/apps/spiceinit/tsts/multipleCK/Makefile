APPNAME = spiceinit

include $(ISISROOT)/make/isismake.tsts

commands:
	cp $(INPUT)/lub5120p.063.lev1.cub $(OUTPUT)/spiceinitTruth.cub > /dev/null;
	$(APPNAME) from=$(OUTPUT)/spiceinitTruth.cub ATTACH=NO CKREC=TRUE CKSMITH=true > /dev/null;
	catlab from=$(OUTPUT)/spiceinitTruth.cub > $(OUTPUT)/labels.pvl;
	rm $(OUTPUT)/spiceinitTruth.cub;
