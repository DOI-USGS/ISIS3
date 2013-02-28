APPNAME = barscale

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/mi15n158e.cub \
	  to=$(OUTPUT)/ontop.cub \
	  rightlimit=100 \
	  rightsegs=2 \
	  cornerline=13000 \
	  cornersample=500 \
	  textloc=above \
	  background=black \
	  pixres=center \
	  units=kilometer \
	  leftlimit=0 \
	  padimage=false \
	  barheight=200 \
	  textheight=75 > /dev/null;
