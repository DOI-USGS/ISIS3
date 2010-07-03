APPNAME = catoriglab

include $(ISISROOT)/make/isismake.tsts

commands:
	mer2isis from=$(INPUT)/test.IMG \
	  to= $(OUTPUT)/mer1.cub > /dev/null;
	$(APPNAME) from= $(OUTPUT)/mer1.cub > $(OUTPUT)/standardOutTruth1.pvl;
