# run spiceinit with multiple quality types of cks allowed.
# 
# This will create a priority queue of Reconstructed and Smithed cks
# from the ck database file that match the time ranges. Based on priority, 
# spiceinit will try to load the kernels.  If this fails, the program will try
# to load the next highest priority.
APPNAME = spiceinit

include $(ISISROOT)/make/isismake.tsts

commands:
	cp $(INPUT)/lub5120p.063.lev1.cub $(OUTPUT)/spiceinitTruth.cub > /dev/null;
	$(APPNAME) from=$(OUTPUT)/spiceinitTruth.cub \
	ATTACH=NO \
	CKREC=TRUE \
	CKSMITH=true > /dev/null;
	catlab from=$(OUTPUT)/spiceinitTruth.cub to=$(OUTPUT)/labels.pvl > /dev/null;
	rm $(OUTPUT)/spiceinitTruth.cub;
