APPNAME = fx

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) f1=$(INPUT)/isisTruth.cub+1 \
	f2=$(INPUT)/isisTruth.cub+2 \
	to=$(OUTPUT)/CUBEresult.cub \
	equation="--f1+f2/cos(1/2)" > /dev/null;
