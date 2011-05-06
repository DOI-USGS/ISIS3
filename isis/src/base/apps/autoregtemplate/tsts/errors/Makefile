.IGNORE:

APPNAME = autoregtemplate

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) \
	  to=$(OUTPUT)/autoregtemplateTruth.txt \
	  algorithm=MaxCor \
	  tolerance=0.7  \
	  psamp=15 \
	  pline=15 \
	  ssamp=31 \
	  sline=31 2> $(OUTPUT)/error.txt > /dev/null;
	$(APPNAME) \
	  to=$(OUTPUT)/autoregtemplateTruth.txt \
	  algorithm=MaximumCorrelation \
	  tolerance=0.7  \
	  reduction=0 \
	  psamp=15 \
	  pline=15 \
	  ssamp=31 \
	  sline=31 2>> $(OUTPUT)/error.txt > /dev/null;
	$(APPNAME) \
	  to=$(OUTPUT)/autoregtemplateTruth.txt \
	  algorithm=MaximumCorrelation \
	  tolerance=0.7  \
	  interpolatortype=C \
	  psamp=15 \
	  pline=15 \
	  ssamp=31 \
	  sline=31 2>> $(OUTPUT)/error.txt > /dev/null;
	$(APPNAME) \
	  to=$(OUTPUT)/autoregtemplateTruth.txt \
	  algorithm=MaximumCorrelation \
	  tolerance=0.7  \
	  chipinterpolator=true \
	  interpolatortype=NONE \
	  psamp=15 \
	  pline=15 \
	  ssamp=31 \
	  sline=31 2>> $(OUTPUT)/error.txt > /dev/null;
	$(APPNAME) \
	  to=$(OUTPUT)/autoregtemplateTruth.txt \
	  algorithm=MaximumCorrelation \
	  tolerance=0.7  \
	  chipinterpolator=true \
	  psamp=15 \
	  pline=15 \
	  ssamp=3 \
	  sline=3 2>> $(OUTPUT)/error.txt > /dev/null;
	$(APPNAME) \
	  to=$(OUTPUT)/autoregtemplateTruth.txt \
	  algorithm=MaximumCorrelation \
	  tolerance=0.7  \
	  chipinterpolator=true  \
	  interpolatortype=cubic \
	  psamp=15 \
	  pline=15 \
	  ssamp=17 \
	  sline=16 2>> $(OUTPUT)/error.txt > /dev/null;
	$(APPNAME) \
	  to=$(OUTPUT)/autoregtemplateTruth.txt \
	  algorithm=MaximumCorrelation \
	  tolerance=0.7  \
	  psamp=15 \
	  pline=15 \
	  ssamp=31 \
	  sline=31 \
	  minimumz=0 2>> $(OUTPUT)/error.txt > /dev/null;
	$(APPNAME) \
	  to=$(OUTPUT)/autoregtemplateTruth.txt \
	  algorithm=MaximumCorrelation \
	  tolerance=0.7  \
	  psamp=15 \
	  pline=15 \
	  ssamp=31 \
	  sline=31 \
	  pvalidpercent=0 2>> $(OUTPUT)/error.txt > /dev/null;
	$(APPNAME) \
	  to=$(OUTPUT)/autoregtemplateTruth.txt \
	  algorithm=MaximumCorrelation \
	  tolerance=0.7  \
	  psamp=15 \
	  pline=15 \
	  ssamp=31 \
	  sline=31 \
	  ssubchipvalidpercent=0 2>> $(OUTPUT)/error.txt > /dev/null;
	$(APPNAME) \
	  to=$(OUTPUT)/autoregtemplateTruth.txt \
	  algorithm=MaximumCorrelation \
	  tolerance=0.7  \
	  psamp=15 \
	  pline=15 \
	  ssamp=31 \
	  sline=31 \
	  subpixel=true \
	  dist=0 2>> $(OUTPUT)/error.txt > /dev/null;
	$(APPNAME) \
	  to=$(OUTPUT)/autoregtemplateTruth.txt \
	  algorithm=MaximumCorrelation \
	  tolerance=0.7  \
	  psamp=15 \
	  pline=15 \
	  ssamp=31 \
	  sline=31 \
	  subpixel=true \
	  window=2 2>> $(OUTPUT)/error.txt > /dev/null;
	$(APPNAME) \
	  to=$(OUTPUT)/autoregtemplateTruth.txt \
	  algorithm=MaximumCorrelation \
	  tolerance=0.7  \
	  psamp=15 \
	  pline=15 \
	  ssamp=31 \
	  sline=31 \
	  subpixel=true \
	  window=4 2>> $(OUTPUT)/error.txt > /dev/null;
	$(APPNAME) \
	  to=$(OUTPUT)/autoregtemplateTruth.txt \
	  algorithm=MaximumCorrelation \
	  tolerance=0.7  \
	  psamp=15 \
	  pline=15 \
	  ssamp=31 \
	  sline=31 \
	  subpixel=true \
	  eccentricitytest=true \
	  eccentricityratio=0 2>> $(OUTPUT)/error.txt > /dev/null;
	$(APPNAME) \
	  to=$(OUTPUT)/autoregtemplateTruth.txt \
	  algorithm=MaximumCorrelation \
	  tolerance=0.7  \
	  psamp=15 \
	  pline=15 \
	  ssamp=31 \
	  sline=31 \
	  subpixel=true \
	  residualtest=true \
	  residualtol=-1 2>> $(OUTPUT)/error.txt > /dev/null;
	cat $(OUTPUT)/error.txt | sed s+/usgs/cpkgs/isis3/testData/isis/src/base/apps/mat2cnet/tsts/errors/++g > $(OUTPUT)/errors.txt;
	rm -f $(OUTPUT)/error.txt;
	rm -f $(OUTPUT)/autoregtemplateTruth.txt;
