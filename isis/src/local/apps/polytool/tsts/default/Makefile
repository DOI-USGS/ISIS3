APPNAME = polytool

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from1=$(INPUT)/multipolygon1.txt \
	to=$(OUTPUT)/despiked.txt > /dev/null;
	$(APPNAME) from1=$(INPUT)/multipolygon1.txt \
	from2=$(INPUT)/multipolygon2.txt process=difference \
	to=$(OUTPUT)/differenced.txt > /dev/null;
	$(APPNAME) from1=$(INPUT)/multipolygon1.txt \
	from2=$(INPUT)/multipolygon2.txt process=intersect \
	to=$(OUTPUT)/intersected.txt > /dev/null;
