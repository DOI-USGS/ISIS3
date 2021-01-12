APPNAME = bandnorm

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/\$$\1.cub \
	  to=$(OUTPUT)/\$$\1.norm.cub average=band -batchlist=$(INPUT)/testInput.lis > /dev/null;
