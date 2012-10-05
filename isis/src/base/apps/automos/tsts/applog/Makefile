APPNAME = automos

include $(ISISROOT)/make/isismake.tsts

commands:
	ls $(INPUT)/*.cub > $(OUTPUT)/inputlist.txt;                                     \
	$(APPNAME) fromlist=$(OUTPUT)/inputlist.txt                                      \
	  mosaic=$(OUTPUT)/automosTruth.cub                                              \
	  tolist=$(OUTPUT)/out.txt                                                       \
	  grange=user minlat=45 maxlat=47.5                                              \
	  minlon=-105 maxlon=265                                                         \
	  -log=$(OUTPUT)/appLogOutput.txt                                                \
	  > /dev/null;                                     \
	  FIRST=`grep -m 1 -n ImageLocation $(OUTPUT)/appLogOutput.txt | sed 's/:.*//'`; \
	  LINECOUNT=`expr "$$FIRST" - 1`; \
	  sed -i -e 1,"$$LINECOUNT"d -e '/ ./!d' $(OUTPUT)/appLogOutput.txt; \
	  `grep -n StartLine $(OUTPUT)/appLogOutput.txt | sed 's/:.*//' > $(OUTPUT)/temp.txt`; \
	  LASTLINE=`sed '$$!d' $(OUTPUT)/temp.txt`; \
	  LAST=`expr "$$LASTLINE" + 1`; \
	  EOF=`wc -l $(OUTPUT)/appLogOutput.txt | sed 's/ .*//'`; \
	  sed -i -e "$$LAST","$$EOF"d $(OUTPUT)/appLogOutput.txt; \
	  cat $(OUTPUT)/out.txt | sed "s#.*/##g" > $(OUTPUT)/outputlist.txt;     \
	  $(RM) $(OUTPUT)/out.txt;                                               \
	  $(RM) $(OUTPUT)/temp.txt; \
	  $(RM) $(OUTPUT)/inputlist.txt;
