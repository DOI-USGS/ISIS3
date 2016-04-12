APP1NAME = reduce
APP2NAME = crop
APP3NAME = cam2map

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APP1NAME) $(TSTARGS) from= $(INPUT)/lua0825f.cub \
	  to= $(OUTPUT)/reduceTruth.cub \
          mode=scale sscale=2.0 lscale=2.0 > /dev/null;
	$(APP2NAME) $(TSTARGS) from= $(OUTPUT)/reduceTruth.cub \
	  to= $(OUTPUT)/ReduceCropTruth.cub \
	  sample=10 nsamples=50 \
	  line=10 nlines=50 > /dev/null;
	$(APP3NAME) $(TSTARGS) from= $(OUTPUT)/ReduceCropTruth.cub \
	  to= $(OUTPUT)/ReduceCropCam2mapTruth.cub \
	  > /dev/null;
	catlab $(TSTARGS) from=$(OUTPUT)/ReduceCropCam2mapTruth.cub > $(OUTPUT)/labels.pvl;
