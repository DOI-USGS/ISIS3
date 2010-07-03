APPNAME = autoregtemplate

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) algorithm=MaximumCorrelation \
	  tolerance=0.7  \
	  psamp=15 \
	  pline=15 \
	  ssamp=31 \
	  sline=31 \
	  to=$(OUTPUT)/autoregtemplateTruth1.txt > /dev/null;
