.IGNORE:

APPNAME = autoregtemplate

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) \
	  topvl=$(OUTPUT)/autoregtemplateTruth.txt \
	  algorithm=MaxCor \
	  tolerance=0.7  \
	  psamp=15 \
	  pline=15 \
	  ssamp=31 \
	  sline=31 2> $(OUTPUT)/error.txt > /dev/null;
	$(APPNAME) \
	  topvl=$(OUTPUT)/autoregtemplateTruth.txt \
	  algorithm=MaximumCorrelation \
	  tolerance=0.7  \
	  reduction=0 \
	  psamp=15 \
	  pline=15 \
	  ssamp=31 \
	  sline=31 2>> $(OUTPUT)/error.txt > /dev/null;
	$(APPNAME) \
	  topvl=$(OUTPUT)/autoregtemplateTruth.txt \
	  algorithm=MaximumCorrelation \
	  tolerance=0.7  \
	  interp=C \
	  psamp=15 \
	  pline=15 \
	  ssamp=31 \
	  sline=31 2>> $(OUTPUT)/error.txt > /dev/null;
	$(APPNAME) \
	  topvl=$(OUTPUT)/autoregtemplateTruth.txt \
	  algorithm=MaximumCorrelation \
	  tolerance=0.7  \
	  interp=NONE \
	  psamp=15 \
	  pline=15 \
	  ssamp=31 \
	  sline=31 2>> $(OUTPUT)/error.txt > /dev/null;
	$(APPNAME) \
	  topvl=$(OUTPUT)/autoregtemplateTruth.txt \
	  algorithm=MaximumCorrelation \
	  tolerance=0.7  \
	  psamp=15 \
	  pline=15 \
	  ssamp=3 \
	  sline=3 2>> $(OUTPUT)/error.txt > /dev/null;
	$(APPNAME) \
	  topvl=$(OUTPUT)/autoregtemplateTruth.txt \
	  algorithm=MaximumCorrelation \
	  tolerance=0.7  \
	  interp=cubic \
	  psamp=15 \
	  pline=15 \
	  ssamp=17 \
	  sline=16 2>> $(OUTPUT)/error.txt > /dev/null;
	$(APPNAME) \
	  topvl=$(OUTPUT)/autoregtemplateTruth.txt \
	  algorithm=MaximumCorrelation \
	  tolerance=0.7  \
	  psamp=15 \
	  pline=15 \
	  ssamp=31 \
	  sline=31 \
	  minimumz=0 2>> $(OUTPUT)/error.txt > /dev/null;
	$(APPNAME) \
	  topvl=$(OUTPUT)/autoregtemplateTruth.txt \
	  algorithm=MaximumCorrelation \
	  tolerance=0.7  \
	  psamp=15 \
	  pline=15 \
	  ssamp=31 \
	  sline=31 \
	  pvalidpercent=0 2>> $(OUTPUT)/error.txt > /dev/null;
	$(APPNAME) \
	  topvl=$(OUTPUT)/autoregtemplateTruth.txt \
	  algorithm=MaximumCorrelation \
	  tolerance=0.7  \
	  psamp=15 \
	  pline=15 \
	  ssamp=31 \
	  sline=31 \
	  ssubchipvalidpercent=0 2>> $(OUTPUT)/error.txt > /dev/null;
	$(APPNAME) \
	  topvl=$(OUTPUT)/autoregtemplateTruth.txt \
	  algorithm=MaximumCorrelation \
	  tolerance=0.7  \
	  psamp=15 \
	  pline=15 \
	  ssamp=31 \
	  sline=31 \
	  subpixel=true \
	  dist=0 2>> $(OUTPUT)/error.txt > /dev/null;
	$(APPNAME) \
	  topvl=$(OUTPUT)/autoregtemplateTruth.txt \
	  algorithm=MaximumCorrelation \
	  tolerance=0.7  \
	  psamp=15 \
	  pline=15 \
	  ssamp=31 \
	  sline=31 \
	  subpixel=true \
	  window=2 2>> $(OUTPUT)/error.txt > /dev/null;
	$(APPNAME) \
	  topvl=$(OUTPUT)/autoregtemplateTruth.txt \
	  algorithm=MaximumCorrelation \
	  tolerance=0.7  \
	  psamp=15 \
	  pline=15 \
	  ssamp=31 \
	  sline=31 \
	  subpixel=true \
	  window=6 2>> $(OUTPUT)/error.txt > /dev/null;
	$(APPNAME) \
	  topvl=$(OUTPUT)/autoregtemplateTruth.txt \
	  algorithm=MaximumCorrelation \
	  tolerance=0.7  \
	  psamp=15 \
	  pline=15 \
	  ssamp=31 \
	  sline=31 \
	  subpixel=true 2>> $(OUTPUT)/error.txt > /dev/null;
	$(APPNAME) \
	  topvl=$(OUTPUT)/autoregtemplateTruth.txt \
	  algorithm=MaximumCorrelation \
	  tolerance=0.7  \
	  psamp=15 \
	  pline=15 \
	  ssamp=31 \
	  sline=31 \
	  subpixel=true > /dev/null;
	rm -f $(OUTPUT)/autoregtemplateTruth.txt;
