APPNAME = lrolola2isis
# This test exercises the ingestion of Lola data  the LRO mission.
# This test is not complete.  The output still needs to be sorted by id and each point entry
# (between {} needs to be moved as a unit.
#
# TODO
# The pointid might need to be entered in single quotes to work ('Lidar????')
# Will the cropped images be sufficient, or do we need the whole image?
#
# 2018-08-31 Debbie Cook - original skeleton of a test

include $(ISISROOT)/make/isismake.tsts

commands:
#	ls $(INPUT)/*.cub > $(OUTPUT)/twoImage.lis
	cp $(INPUT)/*.lis $(OUTPUT)
	$(APPNAME) from=$(INPUT)/LidarTest.csv cubes=$(OUTPUT)/twoImage.lis \
	  to=$(OUTPUT)/LidarTest.json outputtype=test threshold=10  \
	  point_range_sigma=10 point_latitude_sigma=10 point_longitude_sigma=10 \
	  point_radius_sigma=10 pointid=Lidar????  \
	  > /dev/null;
	rm $(OUTPUT)/twoImage.lis
	mv $(OUTPUT)/LidarTest.json $(OUTPUT)/LidarTest.txt
