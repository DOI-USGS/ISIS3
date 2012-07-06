APPNAME = findgaps

include $(ISISROOT)/make/isismake.tsts

# Test 1 - Everything except for the input is set to default. Nothing should happen.
# Test 2 - The log file and the buffer sizes are set to their default values.
# Test 3 - The output cube and the buffer sizes are set to their default values.

commands:
	$(APPNAME) from=$(INPUT)/findGapsTest_basedOnI02025005RDR.cub \
	  log=$(OUTPUT)/cubeIsDefaultLogResults.pvl;                  \
	                                                              \
	$(APPNAME) from=$(INPUT)/findGapsTest_basedOnI02025005RDR.cub \
	  to=$(OUTPUT)/pvlIsDefaultCubeResults.cub;                   \
	                                                              \
	$(APPNAME) from=$(INPUT)/findGapsTest_basedOnI02025005RDR.cub \
	  > /dev/null;
