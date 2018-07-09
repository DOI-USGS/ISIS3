APPNAME = ddd2isis

include $(ISISROOT)/make/isismake.tsts

commands:
	# TEST: Throws an error when trying to open the file
	if [ `$(APPNAME) \
		FROM=$(INPUT)/vis1flat.ddd \
		TO=$(OUTPUT)/vis1flat.cub \
		2> $(OUTPUT)/temp.txt > /dev/null` ]; \
		then true; \
		fi;

	# TEST: Throws an error when trying to read from a cub instead of ddd
	if [ `$(APPNAME) \
		FROM=$(INPUT)/vis1flat.cub \
		TO=$(OUTPUT)/vis1flat.cub \
		2>> $(OUTPUT)/temp.txt > /dev/null` ]; \
		then true; \
		fi;

	# Removes input file path up until input
	$(SED) 's+\[.*/input+[input+' $(OUTPUT)/temp.txt > $(OUTPUT)/errorTruth.txt;
	$(RM) $(OUTPUT)/temp.txt
