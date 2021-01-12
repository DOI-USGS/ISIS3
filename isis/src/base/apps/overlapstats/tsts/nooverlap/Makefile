APPNAME = overlapstats

error.txt.IGNORELINES = Processed

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) $(INPUT)/*.cub > $(OUTPUT)/list.lis;
	if [ `$(APPNAME) from=$(OUTPUT)/list.lis \
	OVERLAPLIST=$(INPUT)/overlaps.lis \
	>& $(OUTPUT)/temp.txt` ]; then \
	  true; \
        fi; \
	$(CAT) $(OUTPUT)/temp.txt | cut -f 1-2 -d "]" \
	> $(OUTPUT)/error.txt;
	$(RM) $(OUTPUT)/temp.txt;
	$(RM) $(OUTPUT)/list.lis;
