APPNAME = crop

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/isisTruth.cub \
	  to=$(OUTPUT)/cropTruth2.cub \
	  sample= 50 \
	  nsamples= 50 \
	  sinc=2 \
	  line= 50 \
	  nlines= 50 \
	  linc=3 > /dev/null;
