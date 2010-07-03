APPNAME = butterworth

include $(ISISROOT)/make/isismake.tsts

commands:
	 $(APPNAME) from=$(INPUT)/testImage.cub \
	  to=$(OUTPUT)/butterworthTruth4.cub \
	  cutoff=50.0 \
	  bandwidth=10.0 \
	  type=bandstop > /dev/null;
