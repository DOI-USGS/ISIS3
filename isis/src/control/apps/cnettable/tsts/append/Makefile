APPNAME = cnettable

include $(ISISROOT)/make/isismake.tsts

commands:
	# This test is for the append function with data as of 2017-04-21
	find $(INPUT)/*.cub > $(OUTPUT)/cubeList.lst; \
	$(APPNAME) FROMLIST=$(OUTPUT)/cubeList.lst \
	CNET=$(INPUT)/testNet.net \
	FLATFILE=$(OUTPUT)/cnetstats.txt \
        > /dev/null;
	# Run again with append mode = true, using same files as above. This should duplicate the same
	# data in the output file.
	$(APPNAME) FROMLIST=$(OUTPUT)/cubeList.lst \
	CNET=$(INPUT)/testNet.net \
	FLATFILE=$(OUTPUT)/cnetstats.txt APPEND=TRUE \
        > /dev/null;      
	rm -f $(OUTPUT)/cubeList.lst > /dev/null;
	cat $(OUTPUT)/cnetstats.txt | $(SED) s/,\[^,]\*\.cub,/,file.cub,/ \
	  | $(SED) 's/\([0-9][0-9]*\.[0-9]\)\([0-9][0-9]*\)/\1/g' \
	  | $(SED) s/`date +%Y-%m-%dT`\[0-2\]\[0-9\]:\[0-5\]\[0-9\]:\[0-5\]\[0-9\]/date/ \
	  > $(OUTPUT)/output.txt
	rm -f $(OUTPUT)/cnetstats.txt > /dev/null;
	# To comment what is going on:
	# The 3 seds do as follows:
	# 1. remove cube filenames
	# 2. remove all but one digit of decimal numbers
	# 3. remove date and time
