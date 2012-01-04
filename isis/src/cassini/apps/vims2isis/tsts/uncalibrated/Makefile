APPNAME = vims2isis

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/v1545949478_1.qub \
	vis=$(OUTPUT)/vimsVisTruth.cub \
        ir=$(OUTPUT)/vimsIrTruth.cub > /dev/null;
	catlab from=$(OUTPUT)/vimsVisTruth.cub \
	  > $(OUTPUT)/vis.pvl;
	catlab from=$(OUTPUT)/vimsIrTruth.cub \
	  > $(OUTPUT)/ir.pvl;
