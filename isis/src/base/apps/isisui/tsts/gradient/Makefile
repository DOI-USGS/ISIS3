APPNAME = isisui

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) gradient \
	  from=$(INPUT)/isisTruth.cub \
	  to=isisTruthSample.cub \
	  gradtype=sobel  > $(OUTPUT)/standardOutTruth2.pvl;
