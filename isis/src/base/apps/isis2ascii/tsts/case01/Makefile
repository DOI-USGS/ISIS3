APPNAME  = isis2ascii

include $(ISISROOT)/make/isismake.tsts

isis2asciiTruth.txt.SKIPLINES = 1

commands:
	$(APPNAME) from=$(INPUT)/isisTruth.cub \
	  to=$(OUTPUT)/isis2asciiTruth.txt > /dev/null;
