APPNAME = butterworth

# 9.6e-07
butterworthTruth2.cub.TOLERANCE = .00000096

include $(ISISROOT)/make/isismake.tsts

commands:
	 $(APPNAME) from=$(INPUT)/testImage.cub \
	  to=$(OUTPUT)/butterworthTruth2.cub \
	  cutoff=50.0 \
	  type=highpass > /dev/null;
