APPNAME =handmos

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/ab102401.lev2.cub \
	mosaic=$(OUTPUT)/handmosTruth2.cub \
	priority=BAND \
	insample=1 \
	inline=1 \
	inband=1 \
	outsample=1 \
	outline=1 \
	outband=1 \
	nlines=1000 \
	nsamples=1000 \
	nbands=5 \
	create=yes \
	propagate=false \
	track=true \
	type=KEYWORD \
	keyname=FilterName \
	keyvalue=RED \
	criteria=LESSER \
	highsaturation=false \
	lowsaturation=false \
	null=false \
	-log=$(OUTPUT)/appLogOutput.txt      \
	> /dev/null;                                                                  \
	FIRST=`grep -m 1 -n ImageLocation $(OUTPUT)/appLogOutput.txt | sed 's/:.*//'`; \
	LINECOUNT=`expr "$$FIRST" - 1`; \
	sed -i -e 1,"$$LINECOUNT"d -e '/ ./!d' $(OUTPUT)/appLogOutput.txt; \
	`grep -n StartLine $(OUTPUT)/appLogOutput.txt | sed 's/:.*//' > $(OUTPUT)/temp.txt`; \
	LASTLINE=`sed '$$!d' $(OUTPUT)/temp.txt`; \
	LAST=`expr "$$LASTLINE" + 1`; \
	EOF=`wc -l $(OUTPUT)/appLogOutput.txt | sed 's/ .*//'`; \
	sed -i -e "$$LAST","$$EOF"d $(OUTPUT)/appLogOutput.txt; \
	$(RM) $(OUTPUT)/temp.txt; 
