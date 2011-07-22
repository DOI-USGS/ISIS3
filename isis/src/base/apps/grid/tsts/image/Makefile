APPNAME = grid

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/peaks.cub \
	to=$(OUTPUT)/imageTruth1.cub mode=image > /dev/null;
	$(APPNAME) from=$(INPUT)/peaks.cub ticks=yes \
	to=$(OUTPUT)/imageTruth2.cub mode=image > /dev/null;
	$(APPNAME) from=$(INPUT)/peaks.cub ticks=yes diagonal=yes \
	to=$(OUTPUT)/imageTruth3.cub mode=image > /dev/null;
