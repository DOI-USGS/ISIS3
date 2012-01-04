APPNAME = vims2isis

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/CM_1514284191_1.cub \
	vis=$(OUTPUT)/vimsVisTruth.cub \
	ir=$(OUTPUT)/vimsIrTruth.cub > /dev/null;
	catlab from=$(OUTPUT)/vimsVisTruth.cub \
	  > $(OUTPUT)/vis.pvl;
	catlab from=$(OUTPUT)/vimsIrTruth.cub \
	  > $(OUTPUT)/ir.pvl;
