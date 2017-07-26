# Test for hayabusa 2 ONC w2 camera. This also tests the user parameter "target".
#
# @history 2017-07-06 Jeannie Backer and Ian Humphrey - Original version. 
#
APPNAME = hyb2onc2isis

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/hyb2_onc_20151203_084458_w2f_l2a.fit \
	           to=$(OUTPUT)/hyb2_onc_20151203_084458_w2f_l2a.cub  \
			   target=Sky                                         \
	           >& /dev/null;
	catlab     from=$(OUTPUT)/hyb2_onc_20151203_084458_w2f_l2a.cub \
	           to=$(OUTPUT)/labels.pvl                            \
	           >& /dev/null;
	catoriglab from=$(OUTPUT)/hyb2_onc_20151203_084458_w2f_l2a.cub \
	           to=$(OUTPUT)/origLab.pvl                           \
	           >& /dev/null;