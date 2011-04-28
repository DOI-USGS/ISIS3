# This test is a simple test of copying the Instruments, Kernels, and BandBin groups
# The tables associated with Kernels already exists in the output
# The important part of this test is that the 3 groups reappear in the output labels.

APPNAME = copylabel

labelsReAdded.pvl.IGNORELINES = Bytes

include $(ISISROOT)/make/isismake.tsts

commands:
	cp $(INPUT)/missInsBBKern.cub $(OUTPUT) > /dev/null;
	$(APPNAME) FROM=$(OUTPUT)/missInsBBKern.cub \
	  SOURCE=$(INPUT)/lub2458i.342.lev1.cub > /dev/null;
	catlab FROM=$(OUTPUT)/missInsBBKern.cub \
	  TO=$(OUTPUT)/labelsReAdded.pvl > /dev/null;

