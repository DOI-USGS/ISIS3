APPNAME = automos

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) fromlist=$(INPUT)/FromList.lst \
	  mosaic=$(OUTPUT)/automosTruth2.cub \
	  priority=beneath > /dev/null;
