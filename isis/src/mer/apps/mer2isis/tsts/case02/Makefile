APPNAME = mer2isis

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/2M189392700EFFAL00P2977M2F1.IMG \
	  to=$(OUTPUT)/mer2isisTruth2.cub > /dev/null;
	catlab from=$(OUTPUT)/mer2isisTruth2.cub > $(OUTPUT)/labels2.pvl;
