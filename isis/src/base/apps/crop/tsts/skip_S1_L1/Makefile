APPNAME = crop

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/isisTruth.cub \
	  to=$(OUTPUT)/cropTruth1.cub \
	  sample= 1 \
	  nsamples= 10 \
	  sinc=1 \
	  line= 3 \
	  nlines= 1 \
	  linc=1 > /dev/null;
