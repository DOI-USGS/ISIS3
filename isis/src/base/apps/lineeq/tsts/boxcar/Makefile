APPNAME = lineeq

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) FROM=$(INPUT)/peaks.cub \
	TO=$(OUTPUT)/truth.cub AVERAGES=yes CSV=$(OUTPUT)/truth.txt \
	boxsize=29 boxtype=percent > /dev/null;
