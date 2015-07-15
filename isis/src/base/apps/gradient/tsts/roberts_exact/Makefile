APPNAME = gradient

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/isisTruth2.cub to=$(OUTPUT)/gradTruth3.cub \
	gradType=ROBERTS method=EXACT > /dev/null;
