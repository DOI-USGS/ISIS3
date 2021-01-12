APPNAME = automos

include $(ISISROOT)/make/isismake.tsts

# Run test.
# Find the first instance of ImageLocation and store the line number in FIRST.
# The number of lines to delete from the start of the file is LINECOUNT = FIRST - 1.
# Find the last intance of StartLine and store the line number in LASTLINE.
# Find the last line number of the last line of the file (EOF).
# Delete from LAST = LASTLINE + 1 to EOF.
# Append End_Group and End onto the end of the pvl.
# Generate outputlist and delete unecessary files.

commands:
	ls $(INPUT)/*.cub > $(OUTPUT)/inputlist.txt;                                           \
	$(APPNAME) fromlist=$(OUTPUT)/inputlist.txt                                            \
	  mosaic=$(OUTPUT)/automosTruth.cub                                                    \
	  tolist=$(OUTPUT)/out.txt                                                             \
	  grange=user minlat=45 maxlat=47.5                                                    \
	  minlon=-105 maxlon=265                                                               \
	  -log=$(OUTPUT)/appLogOutput.pvl                                                      \
	  > /dev/null;                                                                         \
	  FIRST=`grep -m 1 -n ImageLocation $(OUTPUT)/appLogOutput.pvl | sed 's/:.*//'`;       \
	  LINECOUNT=`expr "$$FIRST" - 1`;                                                      \
	  `cat $(OUTPUT)/appLogOutput.pvl | sed -e "1,$${LINECOUNT}d" -e '/ ./!d' > $(OUTPUT)/temp.pvl`;                   \
	  `cat $(OUTPUT)/temp.pvl > $(OUTPUT)/appLogOutput.pvl`;\
	  `grep -n StartLine $(OUTPUT)/appLogOutput.pvl | sed 's/:.*//' > $(OUTPUT)/temp.pvl`; \
	  LASTLINE=`sed '$$!d' $(OUTPUT)/temp.pvl`;                                            \
	  LAST=`expr "$$LASTLINE" + 1`;                                                        \
	  EOF=`wc -l $(OUTPUT)/appLogOutput.pvl | sed 's/\(^ *\)\([0-9]*\)\( .*\$\)/\2/'`;      \
	  cat $(OUTPUT)/appLogOutput.pvl | sed -e "$$LAST","$$EOF"d > $(OUTPUT)/temp.pvl;       \
	  `cat $(OUTPUT)/temp.pvl > $(OUTPUT)/appLogOutput.pvl`;\
	  `echo 'End_Group' >> $(OUTPUT)/appLogOutput.pvl`;                                    \
	  `echo 'End' >> $(OUTPUT)/appLogOutput.pvl`;                                          \
	  cat $(OUTPUT)/out.txt | sed "s#.*/##g" > $(OUTPUT)/outputlist.txt;                   \
	  $(RM) $(OUTPUT)/out.txt;                                                             \
	  $(RM) $(OUTPUT)/temp.pvl;                                                            \
	  $(RM) $(OUTPUT)/inputlist.txt;
