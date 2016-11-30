# @history 2016-10-07 Marjorie Hahn - Turned off barscale's tests for now since barscale requires X11 forwarding to function properly.
#                         The old test data can be found in: 
#                         /work/projects/flytrap/isis-bank/isis3-vault/20161006/tree/testData/isis/src/base/apps/barscale/tsts/ontop

APPNAME = barscale

include $(ISISROOT)/make/isismake.tsts

commands:
	cp $(INPUT)/ontopTruth.txt $(OUTPUT)/ontopTruth.txt;


# OLD TEST COMMANDS:

#	$(APPNAME) from=$(INPUT)/mi15n158e.cub \
#	  to=$(OUTPUT)/ontop.cub \
#	  rightlimit=100 \
#	  rightsegs=2 \
#	  cornerline=12707 \
#	  cornersample=500 \
#	  textloc=below \
#	  background=white \
#	  pixres=center \
#	  units=kilometer \
#	  leftlimit=0 \
#	  padimage=false \
#	  barheight=200 \
#	  textheight=75 >& /dev/null; \
#	crop from=$(OUTPUT)/ontop.cub \
#	  to=$(OUTPUT)/ontop_crop.cub \
#	  sample=1 nsamples=12000 sinc=1 \
#	  line=1 nlines=12917 linc=1 \
#	  > /dev/null; \
#	$(MV) $(OUTPUT)/ontop_crop.cub $(OUTPUT)/ontop.cub;
