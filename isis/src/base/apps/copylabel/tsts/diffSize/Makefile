# Important features of this test:
# The cube we are applying the labels to is a 50% size version of the source file
# and has had nearly all labels removed, everything except for AlphaCube
# which gets deleted by the application. Everything should get copied back in,
# The only things not copied back in would be Mapping and Radiometry which didn't
# exist in the first place. So, labels, and blobs.

APPNAME = copylabel

reducedStripped.pvl.IGNORELINES = Bytes

include $(ISISROOT)/make/isismake.tsts

commands:
	cp $(INPUT)/reducedStripped.cub $(OUTPUT) > /dev/null;
	$(APPNAME) FROM=$(OUTPUT)/reducedStripped.cub \
	  SOURCE=$(INPUT)/lub2458i.342.lev1.cub > /dev/null;
	catlab FROM=$(OUTPUT)/reducedStripped.cub \
	  TO=$(OUTPUT)/reducedStripped.pvl > /dev/null;
