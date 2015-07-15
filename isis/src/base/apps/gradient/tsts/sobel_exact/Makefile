APPNAME = gradient

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/isisTruth2.cub to=$(OUTPUT)/gradTruth4.cub \
	gradType=SOBEL method=EXACT > /dev/null;
