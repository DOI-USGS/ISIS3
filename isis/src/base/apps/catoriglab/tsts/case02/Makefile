APPNAME = catoriglab

include $(ISISROOT)/make/isismake.tsts

commands:
	mer2isis from=$(INPUT)/test2.IMG \
	  to= $(OUTPUT)/mer2.cub > /dev/null;
	$(APPNAME) from= $(OUTPUT)/mer2.cub > $(OUTPUT)/standardOutTruth2.pvl;
