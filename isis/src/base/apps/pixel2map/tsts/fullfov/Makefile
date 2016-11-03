APPNAME = pixel2map

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/VIR_IR_1B_1_506266095_refl_1.b75.l1.cub \
               to=$(OUTPUT)/p2m-VIR_IR_1B_1_506266095_refl_1.b75.l2.full.cub \
	           map=$(INPUT)/equirectangular.map \
			   pixres=mpp \
			   resolution=100 \
			   fovrange=fullexp \
			   > /dev/null;
