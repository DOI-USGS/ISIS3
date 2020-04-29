# Test for new format of Hayabusa2 fits images released 2019-2020
#
# @history 2020-03-29 Kristin Berry - Added along with support for new format
#
APPNAME = hyb2onc2isis

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/hyb2_onc_20180710_060508_tvf_l2a.fit  \
	           to=$(OUTPUT)/hyb2_onc_20180710_060508_tvf_l2a.cub   \
	           >& /dev/null;
	catlab     from=$(OUTPUT)/hyb2_onc_20180710_060508_tvf_l2a.cub \
	           to=$(OUTPUT)/labels.pvl                             \
	           >& /dev/null;
	catoriglab from=$(OUTPUT)/hyb2_onc_20180710_060508_tvf_l2a.cub \
	           to=$(OUTPUT)/origLab.pvl                            \
	           >& /dev/null;