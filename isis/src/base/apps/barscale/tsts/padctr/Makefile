# @history 2016-10-07 Marjorie Hahn - Turned off barscale's tests for now since barscale requires X11 forwarding to function properly.
#                         The old test data can be found in: 
#                         /work/projects/flytrap/isis-bank/isis3-vault/20161006/tree/testData/isis/src/base/apps/barscale/tsts/padctr

APPNAME = barscale

include $(ISISROOT)/make/isismake.tsts

commands:
	cp $(INPUT)/padctrTruth.txt $(OUTPUT)/padctrTruth.txt;


# OLD TEST COMMANDS:

#	$(APPNAME) from=$(INPUT)/EN0213240738M.lev1.cub \
#	  to=$(OUTPUT)/padctr.cub \
#	  rightlimit=20000 \
#	  rightsegs=4 \
#	  textloc=below \
#	  background=black \
#	  pixres=user \
#	  line=267 \
#	  sample=254 \
#	  units=meter \
#	  leftlimit=10000 \
#	  leftsegs=2 \
#	  padimage=yes \
#	  placement=center \
#	  barheight=20 \
#	  textheight=15 >& /dev/null; \
#	crop from=$(OUTPUT)/padctr.cub \
#	  to=$(OUTPUT)/padctr_crop.cub \
#	  sample=1 nsamples=512 sinc=1 \
#	  line=1 nlines=541 linc=1 \
#	  > /dev/null; \
#	$(MV) $(OUTPUT)/padctr_crop.cub $(OUTPUT)/padctr.cub;
