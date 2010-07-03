APPNAME = uncrop

include $(ISISROOT)/make/isismake.tsts

commands:		
	makecube \
	  to=$(OUTPUT)/parent.cub \
	  pixels=null \
	  samples=100 \
	  lines=100 \
	  bands=2 > /dev/null;
	crop \
	  from= $(INPUT)/isisTruth.cub \
	  to=$(OUTPUT)/child.cub \
	  sample=10 \
	  nsamples=20 \
	  sinc=1 \
	  line=10 \
	  nlines=20 \
	  linc=1 > /dev/null;
	$(APPNAME) from=$(OUTPUT)/child.cub \
	  parent=$(OUTPUT)/parent.cub \
	  combine=crop > /dev/null; \
