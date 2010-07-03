APP1NAME = crop
APP2NAME = enlarge
APP3NAME = cam2map

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APP1NAME) from= $(INPUT)/lua0825f.cub \
	  to= $(OUTPUT)/cropTruth.cub \
	  sample=15 nsamples=150 \
	  line=50 nlines=150 > /dev/null;
	$(APP2NAME) from= $(OUTPUT)/cropTruth.cub \
	  to= $(OUTPUT)/CropEnlargeTruth.cub \
	  interp=nearestneighbor sscale=2 lscale=2 > /dev/null;
	$(APP3NAME) from= $(OUTPUT)/CropEnlargeTruth.cub \
	  to= $(OUTPUT)/CropEnlargeCam2mapTruth.cub \
	  > /dev/null;
	catlab from=$(OUTPUT)/CropEnlargeCam2mapTruth.cub > $(OUTPUT)/labels.pvl;
