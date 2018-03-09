APPNAME = tgocassisunstitch

include $(ISISROOT)/make/isismake.tsts

commands:
        # Test output prefix with standard extension (will be removed)
	$(APPNAME) from=$(INPUT)/stitched-2016-11-26T22:50:27.381.cub \
	           outputprefix=$(OUTPUT)/stitched-2016-11-26T22:50:27.381.cub > /dev/null;
	mv $(OUTPUT)/stitched-2016-11-26T22:50:27.381.lis \
	   $(OUTPUT)/stitched-2016-11-26T22:50:27.381.txt;



