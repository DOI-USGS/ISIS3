APPNAME = barscale

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/mi15n158e.cub \
	  to=$(OUTPUT)/ontop.cub \
	  rightlimit=100 \
	  rightsegs=2 \
	  cornerline=12707 \
	  cornersample=500 \
	  textloc=below \
	  background=white \
	  pixres=center \
	  units=kilometer \
	  leftlimit=0 \
	  padimage=false \
	  barheight=200 \
	  textheight=75 >& /dev/null; \
	crop from=$(OUTPUT)/ontop.cub \
	  to=$(OUTPUT)/ontop_crop.cub \
	  sample=1 nsamples=12000 sinc=1 \
	  line=1 nlines=12917 linc=1 \
	  > /dev/null; \
	$(MV) $(OUTPUT)/ontop_crop.cub $(OUTPUT)/ontop.cub;
