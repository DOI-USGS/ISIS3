APP1NAME = msi2isis
APP2NAME = spiceinit
APP3NAME = skypt
APP4NAME = campt
APP5NAME = cam2map
APP6NAME = getsn
APP7NAME = catlab

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APP1NAME) from=$(INPUT)/m0126865998f4_2p.lbl \
	  to=$(OUTPUT)/m0126865998f4_2p.msi2isis.spiceinit.cub > /dev/null;

	$(APP2NAME) from=$(OUTPUT)/m0126865998f4_2p.msi2isis.spiceinit.cub \
	  > /dev/null;

	$(APP3NAME) from=$(OUTPUT)/m0126865998f4_2p.msi2isis.spiceinit.cub \
	  to=$(OUTPUT)/m0126865998f4_2p.msi2isis.spiceinit.skypt_topleftvalid.pvl \
	  SAMPLE=34 LINE=34 > /dev/null;

	$(APP3NAME) from=$(OUTPUT)/m0126865998f4_2p.msi2isis.spiceinit.cub \
	  to=$(OUTPUT)/m0126865998f4_2p.msi2isis.spiceinit.skypt_toprightvalid.pvl \
	  SAMPLE=504 LINE=34 > /dev/null;

	$(APP3NAME) from=$(OUTPUT)/m0126865998f4_2p.msi2isis.spiceinit.cub \
	  to=$(OUTPUT)/m0126865998f4_2p.msi2isis.spiceinit.skypt_botleftvalid.pvl \
	  SAMPLE=34 LINE=379 > /dev/null;

	$(APP4NAME) from=$(OUTPUT)/m0126865998f4_2p.msi2isis.spiceinit.cub \
	  to=$(OUTPUT)/m0126865998f4_2p.msi2isis.spiceinit.campt_botrightvalid.pvl \
	  SAMPLE=504 LINE=379 ALLOWOUTSIDE=no > /dev/null;

	$(APP4NAME) from=$(OUTPUT)/m0126865998f4_2p.msi2isis.spiceinit.cub \
	  to=$(OUTPUT)/m0126865998f4_2p.msi2isis.spiceinit.campt_center.pvl \
	  ALLOWOUTSIDE=no > /dev/null;

	$(APP5NAME) from=$(OUTPUT)/m0126865998f4_2p.msi2isis.spiceinit.cub \
	  to=$(OUTPUT)/m0126865998f4_2p.msi2isis.spiceinit.cam2map.cub \
	  map=$(INPUT)/sinusoidal.map \
	  defaultrange=MAP >/dev/null;

	$(APP4NAME) from=$(OUTPUT)/m0126865998f4_2p.msi2isis.spiceinit.cam2map.cub \
	  to=$(OUTPUT)/m0126865998f4_2p.msi2isis.spiceinit.cam2map.topleft.pvl \
	  SAMPLE=1 LINE=1 ALLOWOUTSIDE=no > /dev/null;

	$(APP4NAME) from=$(OUTPUT)/m0126865998f4_2p.msi2isis.spiceinit.cam2map.cub \
	  to=$(OUTPUT)/m0126865998f4_2p.msi2isis.spiceinit.cam2map.topright.pvl \
	  SAMPLE=87 LINE=1 ALLOWOUTSIDE=no > /dev/null;

	$(APP4NAME) from=$(OUTPUT)/m0126865998f4_2p.msi2isis.spiceinit.cam2map.cub \
	  to=$(OUTPUT)/m0126865998f4_2p.msi2isis.spiceinit.cam2map.botleft.pvl \
	  SAMPLE=1 LINE=165 ALLOWOUTSIDE=no > /dev/null;

	$(APP4NAME) from=$(OUTPUT)/m0126865998f4_2p.msi2isis.spiceinit.cam2map.cub \
	  to=$(OUTPUT)/m0126865998f4_2p.msi2isis.spiceinit.cam2map.botright.pvl \
	  SAMPLE=87 LINE=165 ALLOWOUTSIDE=no > /dev/null;

	$(APP4NAME) from=$(OUTPUT)/m0126865998f4_2p.msi2isis.spiceinit.cam2map.cub \
	  to=$(OUTPUT)/m0126865998f4_2p.msi2isis.spiceinit.cam2map.center.pvl \
	  ALLOWOUTSIDE=no > /dev/null;

	echo -e "\nOutput of getsn..." \
	  > $(OUTPUT)/m0126865998f4_2p.msi2isis.spiceinit.getsn.txt;
	$(APP6NAME) from=$(OUTPUT)/m0126865998f4_2p.msi2isis.spiceinit.cub \
	  >> $(OUTPUT)/m0126865998f4_2p.msi2isis.spiceinit.getsn.txt;

	$(APP7NAME) from=$(OUTPUT)/m0126865998f4_2p.msi2isis.spiceinit.cam2map.cub \
	  to=$(OUTPUT)/m0126865998f4_2p.msi2isis.spiceinit.cam2map.catlab.pvl \
	  > /dev/null;

	$(RM) $(OUTPUT)/m0126865998f4_2p.msi2isis.spiceinit.cub > /dev/null;
	$(RM) $(OUTPUT)/m0126865998f4_2p.msi2isis.spiceinit.cam2map.cub > /dev/null;
	$(RM) $(OUTPUT)/../print.prt > /dev/null;
