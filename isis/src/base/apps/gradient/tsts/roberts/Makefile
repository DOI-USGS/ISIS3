APPNAME = gradient

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/isisTruth.cub \
	  to=$(OUTPUT)/gradTruth2.cub \
	  gradType=Roberts method=APPROXIMATE > /dev/null;
