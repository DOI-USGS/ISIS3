APPNAME = thm2isis

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/I00831002RDR.QUB \
	to=$(OUTPUT)/truth.cub > /dev/null;
	cathist from=$(OUTPUT)/truth.cub \
	to=$(OUTPUT)/historyTruth.pvl > /dev/null;
