APPNAME = thm2isis

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/I00831002RDR.QUB \
	to=$(OUTPUT)/truth.cub > /dev/null;
	cathist from=$(INPUT)/standardOutTruth.pvl.DIFF \
	to=$(OUTPUT)/standardOutTruth.pvl > /dev/null;
