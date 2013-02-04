# This test does not hold any images, lets process=calculate, and solve=SVD.
# This will calculate statistics using the least squares SVD solve method,
# however the statistics will not be applied. 
APPNAME = equalizer

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) $(INPUT)/*.cub > $(OUTPUT)/FromList.lst;
	$(APPNAME) fromlist=$(OUTPUT)/FromList.lst \
	  outstats=$(OUTPUT)/outstats.pvl \
	  process=calculate \
	  solvemethod=sparse \
	  > /dev/null;
	$(RM) $(OUTPUT)/FromList.lst;
