INGEST = hyb2onc2isis
INIT = spiceinit
CAMPT = campt

include $(ISISROOT)/make/isismake.tsts

commands:
	$(INGEST) $(TSTARGS) from= $(INPUT)/hyb2_onc_20151203_072958_w2f_l2a.fit \
	  to= $(OUTPUT)/hyb2_onc_20151203_072958_w2f_l2a.fit.cub > /dev/null

	$(INIT) $(TSTARGS) from= $(OUTPUT)/hyb2_onc_20151203_072958_w2f_l2a.fit.cub \
	  > /dev/null

	# Center of planet
	$(CAMPT) $(TSTARGS) from= $(OUTPUT)/hyb2_onc_20151203_072958_w2f_l2a.fit.cub \
		to= $(OUTPUT)/hyb2_onc_20151203_072958_w2f_l2a.IMG--finalOutput.pvl SAMPLE=400 LINE=750 ALLOWOUTSIDE=no \
		> /dev/null

 	# Left side of planet
	$(CAMPT) $(TSTARGS) from= $(OUTPUT)/hyb2_onc_20151203_072958_w2f_l2a.fit.cub \
	  to= $(OUTPUT)/hyb2_onc_20151203_072958_w2f_l2a.IMG--finalOutput.pvl SAMPLE=318 LINE=760 ALLOWOUTSIDE=no \
	  > /dev/null

	# Bottom of planet
	$(CAMPT) $(TSTARGS) from= $(OUTPUT)/hyb2_onc_20151203_072958_w2f_l2a.fit.cub \
	  to= $(OUTPUT)/hyb2_onc_20151203_072958_w2f_l2a.IMG--finalOutput.pvl SAMPLE=400 LINE=841 ALLOWOUTSIDE=no \
	  > /dev/null

	# Right side of planet
	$(CAMPT) $(TSTARGS) from= $(OUTPUT)/hyb2_onc_20151203_072958_w2f_l2a.fit.cub \
	  to= $(OUTPUT)/hyb2_onc_20151203_072958_w2f_l2a.IMG--finalOutput.pvl SAMPLE=483 LINE=754 ALLOWOUTSIDE=no \
	  > /dev/null

	# Top of planet
	$(CAMPT) $(TSTARGS) from= $(OUTPUT)/hyb2_onc_20151203_072958_w2f_l2a.fit.cub \
	  to= $(OUTPUT)/hyb2_onc_20151203_072958_w2f_l2a.IMG--finalOutput.pvl SAMPLE=400 LINE=680 ALLOWOUTSIDE=no \
	  > /dev/null
		
	$(RM) $(OUTPUT)/hyb2_onc_20151203_072958_w2f_l2a.fit.cub > /dev/null
	$(RM) $(OUTPUT)/../print.prt > /dev/null
