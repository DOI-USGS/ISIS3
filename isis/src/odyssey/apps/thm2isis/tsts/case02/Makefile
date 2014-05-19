APPNAME = thm2isis

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/V00821003RDR.QUB \
	to=$(OUTPUT)/thm2isis-Truth3.cub+32bit > /dev/null;
	cathist from=$(INPUT)/standardOutTruth2.pvl.DIFF \
	to=$(OUTPUT)/standardOutTruth2.pvl > /dev/null;
