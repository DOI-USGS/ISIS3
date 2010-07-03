APP1NAME = crop
APP2NAME = cam2map

labels.txt.IGNORELINES = ByteOrder StartByte Bytes

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APP1NAME) from= $(INPUT)/lua0825f.cub \
	  to= $(OUTPUT)/cropTruth.cub \
	  sample=13 nsamples=100 \
	  line=40 nlines=100 > /dev/null;
	$(APP2NAME) from= $(OUTPUT)/cropTruth.cub \
	  to= $(OUTPUT)/CropCam2mapTruth.cub \
	  > /dev/null;
	catlab from=$(OUTPUT)/CropCam2mapTruth.cub > $(OUTPUT)/labels.pvl;
