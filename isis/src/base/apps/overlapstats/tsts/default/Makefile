APPNAME = overlapstats

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) $(INPUT)/*.cub > $(OUTPUT)/list.lis;
	$(APPNAME) from=$(OUTPUT)/list.lis \
	OVERLAPLIST=$(INPUT)/overlaps.lis \
	| grep -v "Processed" | grep -v "Working" \
	> $(OUTPUT)/brief.pvl;
	$(RM) $(OUTPUT)/list.lis;
