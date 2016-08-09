# This test checks to see that overlaps are properly generated 
# when one image is completely inside of another image.
APPNAME = findimageoverlaps

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) $(INPUT)/*.cub > $(OUTPUT)/cubes.lis;
	$(APPNAME) FROM=$(OUTPUT)/cubes.lis OVERLAPLIST=$(OUTPUT)/overlaps.txt > /dev/null;
	$(RM) $(OUTPUT)/cubes.lis;
