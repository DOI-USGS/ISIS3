# This test will split a control net with 8 points into 3 output control nets
# we should get 
#     outsplit01 with points 1,2,3
#     outsplit02 with points 4,5,6
#     outsplit03 with points 7,8
APPNAME = cnetsplit

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) cnet=$(INPUT)/controlNet.net \
	  onet_prefix=$(OUTPUT)/outsplit?? \
	  num_output_files=3 > /dev/null;
