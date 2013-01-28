# This test does not hold any images, lets process=calculate, and solve=SVD.
# This will calculate statistics using the least squares SVD solve method,
# however the statistics will not be applied. 
APPNAME = equalizer

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) fromlist=$(INPUT)/FromList.lst \
	  outstats=$(OUTPUT)/outstats.pvl \
	  process=calculate \
	  solvemethod=SVD \
	  > /dev/null;

