# Default test for ONC-T calibration. Note that this does not currently account for smear,
# PSF, or allow conversion to Radiance or I/F (since we do not have the calibration parameters
# for these).

APPNAME = hyb2onccal

include $(ISISROOT)/make/isismake.tsts

commands:
	# Test bias, dark current, and flat-field correction for ONC-T image.
	$(APPNAME) \
	FROM=$(INPUT)/hyb2_onc_20151204_040908_tvf_l2a.cub \
	TO=$(OUTPUT)/hyb2_onc_20151204_040908_tvf_l2a.cal.cub \
	> /dev/null;
