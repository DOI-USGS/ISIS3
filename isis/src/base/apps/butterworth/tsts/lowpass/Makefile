APPNAME = butterworth

include $(ISISROOT)/make/isismake.tsts

commands:
	 $(APPNAME) from=$(INPUT)/testImage.cub \
	  to=$(OUTPUT)/butterworthTruth1.cub \
	  cutoff=50.0 \
	  type=lowpass > /dev/null;
