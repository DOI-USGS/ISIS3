APPNAME = fft

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from= $(INPUT)/testImage.cub \
	  magnitude=$(OUTPUT)/fftMagTruth.cub \
	  phase= $(OUTPUT)/fftPhaseTruth.cub > /dev/null;
