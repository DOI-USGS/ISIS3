APP1NAME = crop
APP2NAME = enlarge
APP3NAME = cam2map
CropEnlargeCam2mapTruth.cub.TOLERANCE = 0.00001

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APP1NAME) $(TSTARGS) from= $(INPUT)/lua0825f.cub \
	  to= $(OUTPUT)/cropTruth.cub+real \
	  sample=15 nsamples=150 \
	  line=50 nlines=150 > /dev/null;
	$(APP2NAME) $(TSTARGS) from= $(OUTPUT)/cropTruth.cub \
	  to= $(OUTPUT)/CropEnlargeTruth.cub+real \
	  interp=nearestneighbor sscale=2 lscale=2 > /dev/null;
	$(APP3NAME) $(TSTARGS) from= $(OUTPUT)/CropEnlargeTruth.cub \
	  to= $(OUTPUT)/CropEnlargeCam2mapTruth.cub+real \
	  > /dev/null;
	catlab $(TSTARGS) from=$(OUTPUT)/CropEnlargeCam2mapTruth.cub > $(OUTPUT)/labels.pvl;
