APPNAME = autoregtemplate

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) algorithm=MaximumCorrelation \
	  tolerance=0.7  \
	  subpixelaccuracy=false \
	  interp=b \
	  psamp=15 \
	  pline=15 \
	  ssamp=31 \
	  sline=31 \
	  topvl=$(OUTPUT)/autoregtemplateTruth3.txt > /dev/null;
