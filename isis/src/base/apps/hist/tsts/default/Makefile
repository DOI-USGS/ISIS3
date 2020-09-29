# 2020-06-25 - Kristin Berry - Added a test to check that if nbins is set, it is used.
#   To make the truthdata for this test, I set the number of bins to 25, a number much smaller than 
#   the internal default, and just checked that the output was different from the default test. 

APPNAME = hist
histTruth.txt.IGNORELINES = Cube
histTruthNbins.txt.IGNORELINES = Cube
histTruthMinMax.txt.IGNORELINES = Cube
TEMP = $(OUTPUT)/histTruth.txt
TEMPNBINS = $(OUTPUT)/histTruthNbins.txt
TEMPMINMAX = $(OUTPUT)/histTruthMinMax.txt

include $(ISISROOT)/make/isismake.tsts

commands:
	# Test without additional arguments
	$(APPNAME) from=$(INPUT)/isisTruth.cub \
	  to=$(TEMP) > /dev/null;

	# Test with setting nbins
	$(APPNAME) from=$(INPUT)/isisTruth.cub nbins=25\
	  to=$(TEMPNBINS) > /dev/null;

	# Test with a min and max
	$(APPNAME) from=$(INPUT)/isisTruth.cub\
          to=$(TEMPMINMAX) minimum=0 maximum=255 nbins=255> /dev/null;
