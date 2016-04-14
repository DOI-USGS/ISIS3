APP1NAME = enlarge
APP2NAME = lowpass
APP3NAME = crop
APP4NAME = cam2map

labels.txt.IGNORELINES = ByteOrder StartByte Bytes

lowpassTruth.cub.TOLERANCE = 8

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APP1NAME) $(TSTARGS) from= $(INPUT)/lua0825f.cub \
	  to= $(OUTPUT)/enlargeTruth.cub+Real \
	  interp=bilinear sscale=1.5 lscale=1.5 > /dev/null;
	$(APP2NAME) $(TSTARGS) from= $(OUTPUT)/enlargeTruth.cub \
	  to= $(OUTPUT)/lowpassTruth.cub \
          samples=31 lines=31 > /dev/null;
	$(RM) $(OUTPUT)/enlargeTruth.cub > /dev/null;
	$(APP3NAME) $(TSTARGS) from= $(OUTPUT)/lowpassTruth.cub \
	  to= $(OUTPUT)/EnlargeCropTruth.cub \
	  sample=50 nsamples=200 \
	  line=100 nlines=200 > /dev/null;
	$(APP4NAME) $(TSTARGS) from= $(OUTPUT)/EnlargeCropTruth.cub \
	  to= $(OUTPUT)/EnlargeCropCam2mapTruth.cub \
	  > /dev/null;
	catlab $(TSTARGS) from=$(OUTPUT)/EnlargeCropCam2mapTruth.cub > $(OUTPUT)/labels.pvl;
