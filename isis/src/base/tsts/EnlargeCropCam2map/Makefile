APP1NAME = enlarge
APP2NAME = crop
APP3NAME = cam2map

labels.txt.IGNORELINES = ByteOrder StartByte Bytes

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APP1NAME) from= $(INPUT)/lua0825f.cub \
	  to= $(OUTPUT)/enlargeTruth.cub \
	  interp=nearestneighbor sscale=1.5 lscale=1.5 > /dev/null;
	$(APP2NAME) from= $(OUTPUT)/enlargeTruth.cub \
	  to= $(OUTPUT)/EnlargeCropTruth.cub \
	  sample=50 nsamples=200 \
	  line=100 nlines=200 > /dev/null;
	$(APP3NAME) from= $(OUTPUT)/EnlargeCropTruth.cub \
	  to= $(OUTPUT)/EnlargeCropCam2mapTruth.cub \
	  > /dev/null;
	catlab from=$(OUTPUT)/EnlargeCropCam2mapTruth.cub > $(OUTPUT)/labels.pvl;
