APPNAME = pca

pcaTransformTruth.cub.TOLERANCE = .00000001

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from= $(INPUT)/test.cub \
	  to= $(OUTPUT)/pcaTransformTruth.cub \
	  mode=transform > /dev/null;
