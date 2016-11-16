APPNAME = camtest

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/lor_0034974380_0x630_sci_1.cub \
	  to= $(OUTPUT)/camtestTruth.cub \
	  format="LON" \
	  -log=$(OUTPUT)/results.pvl > /dev/null; \
	  FIRST=`grep -m 1 -n "CamTestResults" $(OUTPUT)/results.pvl | sed 's/:.*//'`; \
	  STARTLINE=`expr "$$FIRST" - 1`; \
	  `cat $(OUTPUT)/results.pvl  | sed "1,$${STARTLINE}d" > $(OUTPUT)/temp.pvl`; \
	  LASTLINE=`grep -m 1 -n "End_Group" $(OUTPUT)/temp.pvl | sed 's/:.*//'`; \
	  ENDLINE=`expr "$$LASTLINE" + 1`; \
	  EOF=`wc -l $(OUTPUT)/temp.pvl | sed 's/\(^ *\)\([0-9]*\)\( .*\$\)/\2/'`; \
	  `cat $(OUTPUT)/temp.pvl | sed "$${ENDLINE},$${EOF}d" > $(OUTPUT)/results.pvl`; \
	  $(RM) $(OUTPUT)/temp.pvl;

