APPNAME = lrolola2isis
# This test exercises the ingestion of Lola data  the LRO mission.
# The output is sorted by id to have consistent output for comparison
# with the truth data.  Normal behavior fills the file from a hash so
# the order varies.
#
# 2018-08-31 Debbie A. Cook - original skeleton of a test

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
