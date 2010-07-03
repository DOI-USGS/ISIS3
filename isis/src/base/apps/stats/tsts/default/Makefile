APPNAME = stats

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/isisTruth.cub \
	  to=$(OUTPUT)/stats.pvl > /dev/null;
	$(APPNAME) from=$(INPUT)/isisTruth.cub \
	  validmin=-20 validmax=20 to=$(OUTPUT)/stats2.pvl > /dev/null;
