APP1NAME = crop
APP2NAME = reduce
APP3NAME = cam2map

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APP1NAME) from= $(INPUT)/lua0825f.cub \
	  to= $(OUTPUT)/cropTruth.cub \
	  sample=15 nsamples=200 \
	  line=50 nlines=200 > /dev/null;
	$(APP2NAME) from= $(OUTPUT)/cropTruth.cub \
	  to= $(OUTPUT)/CropReduceTruth.cub \
	  mode=total \
	  ons=150 onl=150 > /dev/null;
	$(APP3NAME) from= $(OUTPUT)/CropReduceTruth.cub \
	  to= $(OUTPUT)/CropReduceCam2mapTruth.cub \
	  > /dev/null;
	catlab from=$(OUTPUT)/CropReduceCam2mapTruth.cub \
	  > $(OUTPUT)/labels.pvl;
