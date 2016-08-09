# This tests to make sure that the proper exception is 
# thrown if no overlaps are present in the data set
APPNAME = findimageoverlaps

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) $(INPUT)/*.cub > $(OUTPUT)/cubes.lis;
	
	if [[ `$(APPNAME) \
	  FROM=$(OUTPUT)/cubes.lis \
	  OVERLAPLIST=$(OUTPUT)/overlaps.txt \
	  2>> $(OUTPUT)/errors.txt \
	  > /dev/null` ]]; \
	then \
	  true; \
	fi;
	
	$(RM) $(OUTPUT)/cubes.lis;
