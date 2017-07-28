# Test bias and dark current correction for ONC-W2 calibration for images without onboard smear correction. 
# Note that this does not currently account for smear,
# PSF, or allow conversion to Radiance or I/F (since we do not have the calibration parameters
# for these).
#
# We do not currently have a valid flatfield for W1 or W2 images, so no flatfield correction
# will be applied.
APPNAME = hyb2onccal

include $(ISISROOT)/make/isismake.tsts

commands:
	# Test bias and dark current correction for ONC-W2 image without smear correction.
	$(APPNAME) \
	FROM=$(INPUT)/hyb2_onc_20151203_085958_w2f_l2a.cub \
	TO=$(OUTPUT)/hyb2_onc_20151203_085958_w2f_l2a.cal.cub \
	> /dev/null;
