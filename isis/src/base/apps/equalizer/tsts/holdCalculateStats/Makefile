# This test passes in a holdlist, lets process=calculate.
# This will calculate the statistics, but not apply them. 
# For this test, the program is run three times - once for each adjust option: 
# contrast, brightness, and both.
APPNAME = equalizer

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) fromlist=$(INPUT)/FromList.lst \
	  holdlist=$(INPUT)/HoldList.lst \
	  outstats=$(OUTPUT)/outstats_contrast.pvl \
	  process=calculate \
	  adjust=contrast \
	  solvemethod=qrd \
	  > /dev/null;
	$(APPNAME) fromlist=$(INPUT)/FromList.lst \
	  holdlist=$(INPUT)/HoldList.lst \
	  outstats=$(OUTPUT)/outstats_brightness.pvl \
	  process=calculate \
	  adjust=brightness \
	  solvemethod=qrd \
	  > /dev/null;
	$(APPNAME) fromlist=$(INPUT)/FromList.lst \
	  holdlist=$(INPUT)/HoldList.lst \
	  outstats=$(OUTPUT)/outstats_both.pvl \
	  process=calculate \
	  solvemethod=qrd \
	  > /dev/null;
