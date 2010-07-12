APPNAME = lronaccal
FILE=nacr00015d79

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/$(FILE).cub \
	  to=$(OUTPUT)/$(FILE).cal.cub > /dev/null;
	crop from=$(OUTPUT)/$(FILE).cal.cub \
	  to=$(OUTPUT)/$(FILE).cal.crop.cub \
	  nlines=100 > /dev/null;
	$(RM) $(OUTPUT)/$(FILE).cal.cub;
