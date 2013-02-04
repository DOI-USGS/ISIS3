APPNAME = autoregtemplate

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) algorithm=MinimumDifference \
	  tolerance=0.7 \
	  reductionfactor=2 \
	  interp=NEARESTNEIGHBORTYPE \
	  psamp=15 \
	  pline=15 \
	  pmin=1200 \
	  pmax=2800 \
	  minimumzscore = 0.5 \
	  pvalidpercent=75 \
	  ssamp=31 \
	  sline=31 \
	  smin=1200 \
	  smax=2800 \
	  ssubchipvalidpercent=75 \
	  distancetolerance=2.5 \
	  windowsize=11 \
	  topvl=$(OUTPUT)/autoregtemplateTruth2.txt > /dev/null;
