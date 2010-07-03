APPNAME = crop

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/isisTruth.cub \
	  to=$(OUTPUT)/cropTruth3.cub \
	  sample= 5 \
	  nsamples= 10 \
	  sinc=5 \
	  line= 5 \
	  nlines= 10 \
	  linc=5 > /dev/null;
