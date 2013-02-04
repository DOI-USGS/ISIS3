APPNAME = maptrim

include $(ISISROOT)/make/isismake.tsts

# Running two instances of maptrim in parallel to make sure the temp files
# do not conflict. This test uses vesta_global_october10_2012.cub as input
# because it is large enoung to make maptrim run long enough for there to be
# potential temp file conflicts.

commands:
	$(APPNAME) from=$(INPUT)/vesta_global_october10_2012.cub      \
	  to=$(OUTPUT)/cropAndTrimOutput1.cub mode=both               \
	  minlat=-5 maxlat=22 minlon=175 maxlon=220> /dev/null &      \
	$(APPNAME) from=$(INPUT)/vesta_global_october10_2012.cub      \
	  to=$(OUTPUT)/cropAndTrimOutput2.cub mode=both               \
	  minlat=-35 maxlat=-25 minlon=224 maxlon=233> /dev/null &    \
	wait;                                                         \
	`echo 'The existance of this file is proof that this test passed.' > $(OUTPUT)/passed.txt`;