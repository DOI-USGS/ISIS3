APPNAME = mapmos

include $(ISISROOT)/make/isismake.tsts


commands:
	cp $(INPUT)/mosaic.cub $(OUTPUT)
	$(APPNAME) from=$(INPUT)/lub4618n.342.lev2.cub                                         \
	  mosaic=$(OUTPUT)/mosaic.cub                                                          \
	  priority=ontop                                                                       \
	  matchbandbin=true                                                                    \
	  -log=$(OUTPUT)/appLogOutput.pvl                                                      \
	  > /dev/null;                                                                         \
	  FIRST=`grep -m 1 -n ImageLocation $(OUTPUT)/appLogOutput.pvl | sed 's/:.*//'`;       \
	  LINECOUNT=`expr "$$FIRST" - 1`;                                                      \
	  sed -i -e 1,"$$LINECOUNT"d -e '/ ./!d' $(OUTPUT)/appLogOutput.pvl;                   \
	  `grep -n StartLine $(OUTPUT)/appLogOutput.pvl | sed 's/:.*//' > $(OUTPUT)/temp.txt`; \
	  LASTLINE=`sed '$$!d' $(OUTPUT)/temp.txt`;                                            \
	  LAST=`expr "$$LASTLINE" + 1`;                                                        \
	  EOF=`wc -l $(OUTPUT)/appLogOutput.pvl | sed 's/ .*//'`;                              \
	  sed -i -e "$$LAST","$$EOF"d $(OUTPUT)/appLogOutput.pvl;                              \
	  `echo 'End_Group' >> $(OUTPUT)/appLogOutput.pvl`;                                    \
	  `echo 'End' >> $(OUTPUT)/appLogOutput.pvl`;                                          \
	$(RM) $(OUTPUT)/temp.txt;
