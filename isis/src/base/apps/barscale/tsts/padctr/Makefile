APPNAME = barscale

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/EN0213240738M.lev1.cub \
	  to=$(OUTPUT)/padctr.cub \
	  rightlimit=20000 \
	  rightsegs=4 \
	  textloc=below \
	  background=black \
	  pixres=user \
	  line=267 \
	  sample=254 \
	  units=meter \
	  leftlimit=10000 \
	  leftsegs=2 \
	  padimage=yes \
	  placement=center \
	  barheight=20 \
	  textheight=10 > /dev/null; \
	crop from=$(OUTPUT)/padctr.cub \
	  to=$(OUTPUT)/padctr_crop.cub \
	  sample=1 nsamples=512 sinc=1 \
	  line=1 nlines=540 linc=1 \
	  > /dev/null; \
	$(MV) $(OUTPUT)/padctr_crop.cub \
	  $(OUTPUT)/padctr.cub;
