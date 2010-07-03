APPNAME = hiccdstitch

include $(ISISROOT)/make/isismake.tsts

commands:
	t=`ls $(INPUT)/PSP_001446_1790_RED?.cub > $(OUTPUT)/list.temp`; \
	$(APPNAME) fromlist=$(OUTPUT)/list.temp \
	  to=$(OUTPUT)/combined.cub \
	  placement=$(OUTPUT)/placement.pvl > /dev/null; \
	t=`rm -f $(OUTPUT)/list.temp`;
