# Test for hayabusa 2 ONC w1 camera
#
# @history 2017-07-06 Jeannie Backer and Ian Humphrey - Original version. 
#
APPNAME = hyb2onc2isis

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/hyb2_onc_20151204_041027_w1f_l2a.fit \
	           to=$(OUTPUT)/hyb2_onc_20151204_041027_w1f_l2a.cub  \
	           >& /dev/null;
	catlab     from=$(OUTPUT)/hyb2_onc_20151204_041027_w1f_l2a.cub \
	           to=$(OUTPUT)/labels.pvl                            \
	           >& /dev/null;
	catoriglab from=$(OUTPUT)/hyb2_onc_20151204_041027_w1f_l2a.cub \
	           to=$(OUTPUT)/origLab.pvl                           \
	           >& /dev/null;