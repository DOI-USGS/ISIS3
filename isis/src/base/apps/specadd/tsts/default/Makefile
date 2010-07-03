APPNAME = specadd

include $(ISISROOT)/make/isismake.tsts

commands:
	makecube to=$(OUTPUT)/makecube.cub value=0.0 samples=126 lines=126 bands=2 > /dev/null;
	$(APPNAME) from=$(OUTPUT)/makecube.cub \
	  match= $(INPUT)/isisTruth.cub \
	  to= $(OUTPUT)/specaddTruth.cub > /dev/null;
