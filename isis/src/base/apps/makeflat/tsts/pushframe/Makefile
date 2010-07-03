APPNAME = makeflat

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) fromlist=$(INPUT)/fromList.lis \
	  to=$(OUTPUT)/pushFrameFlatTruth.cub STDEV=150 \
          IMAGETYPE=PUSHFRAME FRAMELETHEIGHT=50 EXCLUDE=$(OUTPUT)/pushFrame.txt \
	   > /dev/null;
