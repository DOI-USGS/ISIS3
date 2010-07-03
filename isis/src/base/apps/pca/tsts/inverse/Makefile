APPNAME = pca

pcaInverseTruth.cub.TOLERANCE = 0.00000000001 
pcaTransformTruth.cub.TOLERANCE = 0.00000001 

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from= $(INPUT)/test.cub \
	  to= $(OUTPUT)/pcaTransformTruth.cub \
	  mode=transform > /dev/null;
	$(APPNAME) from= $(OUTPUT)/pcaTransformTruth.cub \
	  to= $(OUTPUT)/pcaInverseTruth.cub \
	  mode=inverse > /dev/null;
