APPNAME = tgocassisunstitch

include $(ISISROOT)/make/isismake.tsts

commands:
        # Test output prefix with standard extension (will be removed)
	$(APPNAME) from=$(INPUT)/CAS-MCO-2016-11-22T16:10:43.505.cub \
	           outputprefix=$(OUTPUT)/CAS-MCO_2016-11-22T16:10:43.505.cub > /dev/null;
	mv $(OUTPUT)/CAS-MCO_2016-11-22T16:10:43.505.lis \
	   $(OUTPUT)/CAS-MCO_2016-11-22T16:10:43.505.txt;



