APPNAME = unstitch

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/CAS-MCO_2016-11-22T16:10:43.505.cub \
	           to=$(OUTPUT)/CAS-MCO_2016-11-22T16:10:43.505.cub > /dev/null;
	mv $(OUTPUT)/CAS-MCO_2016-11-22T16:10:43.505.lis \
	   $(OUTPUT)/CAS-MCO_2016-11-22T16:10:43.505.txt;
