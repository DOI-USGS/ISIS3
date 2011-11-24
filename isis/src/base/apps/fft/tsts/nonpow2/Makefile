APPNAME = fft

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from= $(INPUT)/3857S.cub+1 \
	  magnitude=$(OUTPUT)/3857S_mag_truth.cub \
	  phase= $(OUTPUT)/3857S_pha_truth.cub > /dev/null;
