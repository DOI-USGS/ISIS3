# Tests the ERRORS option. The application should write errors to the specified
# errors file.
APPNAME = overlapstats

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) $(INPUT)/*.cub > $(OUTPUT)/list.lis;
	$(APPNAME) FROM=$(OUTPUT)/list.lis \
	  ERRORS=$(OUTPUT)/errors.txt \
	  OVERLAPLIST=$(INPUT)/overlaps.lis \
	  > /dev/null;
	$(RM) $(OUTPUT)/list.lis;
