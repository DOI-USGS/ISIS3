APPNAME = apinit

include $(ISISROOT)/make/isismake.tsts

commands:
	std2isis from=$(INPUT)/N10NAPPW06666039.tif to=$(OUTPUT)/N10NAPPW06666039.cub > /dev/null;
	$(APPNAME) from=$(OUTPUT)/N10NAPPW06666039.cub \
	           DATETIME="1999-06-12T12:05" ALTITUDE=6096 \
	           CENTERLAT=39.9363 CENTERLON=255.285823 \
                   SHAPEMODEL=$(INPUT)/dtm.cub FOCALLENGTH=152.818 \
                   FIDUCIALX="-106.0, -0.003, 105.999, -110.005, 109.999, -106.008, -0.003, 105.998" \
                   FIDUCIALY="106.000, 109.999, 105.994, 0.002, 0.002, -105.999, -110.004, -105.999" \
                   FIDUCIALSAMPLE="574, 2040, 3500, 513, 3562, 578, 2039, 3497" \
                   FIDUCIALLINE="255, 194, 261, 1721, 1721, 3182, 3245, 3180" \
                   KCOEFS="1.27485E-05, 3.73255E-09, -6.95721E-13, 2.3884E-17, 0.0" \
                   PCOEFS="0, 0, 0, 0" XP=-0.004 YP=0 XIPP=-0.002 YIPP=0.001 > /dev/null;
	catlab from=$(OUTPUT)/N10NAPPW06666039.cub to=$(OUTPUT)/N10NAPPW06666039.pvl > /dev/null;
