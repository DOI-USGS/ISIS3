APPNAME = cnetwinnow

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) -1 $(INPUT)/*.cub > $(OUTPUT)/cube.lis;
	$(APPNAME) fromlist=$(OUTPUT)/cube.lis \
	cnet=$(INPUT)/inputNet.net \
	onet=$(OUTPUT)/winnowed.net \
        file_prefix = winnowingReport > /dev/null;
	cat winnowingReportGuilty.csv \
	  | perl -pe 's/(^|,|: )([^,:]+\/)([^,\/:]*\.)(net|cub)/\1\3\4/g' 2>/dev/null \
	  | sed 's/\([0-9][0-9]*\.[0-9][0-9][0-9][0-9][0-9]\)\([0-9][0-9]*\)/\1/g' \
	  > $(OUTPUT)/winnowingReportGuilty.csv;
	cat winnowingReportIgnored.csv \
	  | perl -pe 's/(^|,|: )([^,:]+\/)([^,\/:]*\.)(net|cub)/\1\3\4/g' 2>/dev/null \
	  | sed 's/\([0-9][0-9]*\.[0-9][0-9][0-9][0-9][0-9]\)\([0-9][0-9]*\)/\1/g' \
	  > $(OUTPUT)/winnowingReportIgnored.txt.csv;
	# The above command uses sed to do the following (in order):
	# 1. remove cube filename paths
	# 3. remove digits beyond the fifth decimal place of decimal numbers
	# 4. remove date and time
	$(RM) winnowingReportGuilty.csv > /dev/null;
	$(RM) winnowingReportIgnored.csv > /dev/null;
	$(RM) $(OUTPUT)/cube.lis > /dev/null;
