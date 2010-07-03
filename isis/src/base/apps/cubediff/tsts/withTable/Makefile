APPNAME = cubediff

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/cubediffTest1.cub \
	  from2=$(INPUT)/cubediffTest2.cub \
	  to=$(OUTPUT)/cubediffTruth3.txt \
	  outputdiffs=true \
	  count=20 > /dev/null;
