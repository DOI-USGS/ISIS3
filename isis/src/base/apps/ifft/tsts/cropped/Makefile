# This tests running ifft on a cube that has been cropped before being run through fft.
# crop -> fft -> ifft
# This is testing that ifft retains the original AlphaCube group from the crop.

# 7.2e-15
ifftTruth.cub.TOLERANCE = .0000000000000072

APPNAME = ifft

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) magnitude=$(INPUT)/PSP_mag.cub \
	  phase=$(INPUT)/PSP_phase.cub \
	  to=$(OUTPUT)/ifftTruth.cub \
	  > /dev/null;
	catlab from=$(OUTPUT)/ifftTruth.cub > $(OUTPUT)/ifftTruth.pvl;
	$(RM) print.prt > /dev/null;
