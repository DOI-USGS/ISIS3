APPNAME = findgaps

include $(ISISROOT)/make/isismake.tsts

# All parameters changed from their default value.

commands:
	$(APPNAME) from=$(INPUT)/findGapsTest_realData.cub                        \
	to=$(OUTPUT)/realDataCubeResults.cub log=$(OUTPUT)/realDataLogResults.pvl \
	cortol=0.6 above=10 below=5                                               \
	> /dev/null;
