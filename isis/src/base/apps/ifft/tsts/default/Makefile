APPNAME = ifft

# 7.2e-15
ifftTruth.cub.TOLERANCE = .0000000000000072

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) magnitude=$(INPUT)/testMag.cub \
	  phase=$(INPUT)/testPhase.cub \
	  to=$(OUTPUT)/ifftTruth.cub > /dev/null;
