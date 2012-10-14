APPNAME =handmos

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/ab102401.lev2.cub                                           \
	mosaic=$(OUTPUT)/handmosTruth2.cub                                                   \
	priority=BAND                                                                        \
	insample=1                                                                           \
	inline=1                                                                             \
	inband=1                                                                             \
	outsample=1                                                                          \
	outline=1                                                                            \
	outband=1                                                                            \
	nlines=1000                                                                          \
	nsamples=1000                                                                        \
	nbands=5                                                                             \
	create=yes                                                                           \
	propagate=false                                                                      \
	track=true                                                                           \
	type=KEYWORD                                                                         \
	keyname=FilterName                                                                   \
	keyvalue=RED                                                                         \
	criteria=LESSER                                                                      \
	highsaturation=false                                                                 \
	lowsaturation=false                                                                  \
	null=false                                                                           \
	-log=$(OUTPUT)/appLogOutput.pvl                                                      \
	> /dev/null;                                                                         \
	FIRST=`grep -m 1 -n ImageLocation $(OUTPUT)/appLogOutput.pvl | sed 's/:.*//'`;       \
	LINECOUNT=`expr "$$FIRST" - 1`;                                                      \
	`cat $(OUTPUT)/appLogOutput.pvl | sed -e "1,$${LINECOUNT}d" -e '/ ./!d' > $(OUTPUT)/temp.pvl`;                   \
	`cat $(OUTPUT)/temp.pvl > $(OUTPUT)/appLogOutput.pvl`;\
	`grep -n StartLine $(OUTPUT)/appLogOutput.pvl | sed 's/:.*//' > $(OUTPUT)/temp.pvl`; \
	LASTLINE=`sed '$$!d' $(OUTPUT)/temp.pvl`;                                            \
	LAST=`expr "$$LASTLINE" + 1`;                                                        \
	EOF=`wc -l $(OUTPUT)/appLogOutput.pvl | sed 's/\(^ *\)\([0-9]*\)\( .*\$\)/\2/'`;     \
	cat $(OUTPUT)/appLogOutput.pvl | sed -e "$$LAST","$$EOF"d > $(OUTPUT)/temp.pvl;       \
	`cat $(OUTPUT)/temp.pvl > $(OUTPUT)/appLogOutput.pvl`;\
	`echo 'End_Group' >> $(OUTPUT)/appLogOutput.pvl`;                                    \
	`echo 'End' >> $(OUTPUT)/appLogOutput.pvl`;                                          \
	$(RM) $(OUTPUT)/temp.pvl; 
