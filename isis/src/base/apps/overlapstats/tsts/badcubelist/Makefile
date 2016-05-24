# Tests that overlapstats correctly throws an exception if the cube list is
# different from the one used to generate the overlap list in findimageoverlaps.
APPNAME = overlapstats

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) $(INPUT)/*.cub > $(OUTPUT)/list.lis;
	if [ `$(APPNAME) from=$(OUTPUT)/list.lis \
	OVERLAPLIST=$(INPUT)/overlaps.lis \
	>& $(OUTPUT)/error.txt` ]; then \
	  true; \
        fi; \
	$(RM) $(OUTPUT)/list.lis;
