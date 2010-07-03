APPNAME = mical

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/2M189392700EFFAL00P2977M2F1.cub \
	   to=$(OUTPUT)/micalTruth1.cub > /dev/null;
	catlab from=$(OUTPUT)/micalTruth1.cub > $(OUTPUT)/labels1.pvl;
