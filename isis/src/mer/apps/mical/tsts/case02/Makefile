APPNAME = mical

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/1M189529263EFF64KCP2977M2F1.cub \
	  to=$(OUTPUT)/micalTruth2.cub > /dev/null;
	catlab from=$(OUTPUT)/micalTruth2.cub > $(OUTPUT)/labels2.pvl;
