APPNAME = lrolola2isis
# This test exercises the ingestion of Lola data from the LRO mission
# with multiple input files. The output is sorted by id to have consistent 
# output for comparison with the truth data.  Normal behavior fills the 
# file from a hash so the order varies.
#
# 2019-03-13 Debbie A. Cook - original test

include $(ISISROOT)/make/isismake.tsts

commands:
#	ls $(INPUT)/*.cub > $(OUTPUT)/multipleInputs.lis
	cp $(INPUT)/*.lis $(OUTPUT)
	$(APPNAME) fromlist=$(OUTPUT)/shortCsv.lis cubes=$(OUTPUT)/shortMultipleInput.lis \
	  to=$(OUTPUT)/LidarTest.json outputtype=test threshold=10  \
	  point_range_sigma=10 point_latitude_sigma=10 point_longitude_sigma=10 \
	  point_radius_sigma=10 pointid=Lidar????  \
	  > /dev/null;
	rm $(OUTPUT)/*.lis
	mv $(OUTPUT)/LidarTest.json $(OUTPUT)/LidarTest.txt
