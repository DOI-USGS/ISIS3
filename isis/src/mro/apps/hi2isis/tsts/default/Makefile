APPNAME = hi2isis

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/PSP_001446_1790_BG12_0.IMG \
	  to=$(OUTPUT)/hi2isis_BG0.cub > /dev/null;
	 $(APPNAME) from=$(INPUT)/PSP_001446_1790_BG12_1.IMG \
	  to=$(OUTPUT)/hi2isis_BG1.cub > /dev/null;
	 $(APPNAME) from=$(INPUT)/PSP_001446_1790_IR11_0.IMG \
	  to=$(OUTPUT)/hi2isis_IR0.cub > /dev/null;
	 $(APPNAME) from=$(INPUT)/PSP_001446_1790_IR11_1.IMG \
	  to=$(OUTPUT)/hi2isis_IR1.cub > /dev/null;
	 $(APPNAME) from=$(INPUT)/PSP_001446_1790_RED2_0.IMG \
	  to=$(OUTPUT)/hi2isis_RED0.cub > /dev/null;
	 $(APPNAME) from=$(INPUT)/PSP_001446_1790_RED2_1.IMG \
	  to=$(OUTPUT)/hi2isis_RED1.cub > /dev/null;

	  tabledump from=$(OUTPUT)/hi2isis_BG0.cub \
	  to=$(OUTPUT)/BG0_CalibrationImage.txt \
	  name="HiRISE Calibration Image" >/dev/null;
	  tabledump from=$(OUTPUT)/hi2isis_BG0.cub \
	  to=$(OUTPUT)/BG0_CalibrationAncillary.txt \
	  name="HiRISE Calibration Ancillary" >/dev/null;
	  tabledump from=$(OUTPUT)/hi2isis_BG0.cub \
	  to=$(OUTPUT)/BG0_Ancillary.txt \
	  name="HiRISE Ancillary" >/dev/null;

	  tabledump from=$(OUTPUT)/hi2isis_BG1.cub \
	  to=$(OUTPUT)/BG1_CalibrationImage.txt \
	  name="HiRISE Calibration Image" >/dev/null;
	  tabledump from=$(OUTPUT)/hi2isis_BG1.cub \
	  to=$(OUTPUT)/BG1_CalibrationAncillary.txt \
	  name="HiRISE Calibration Ancillary" >/dev/null;
	  tabledump from=$(OUTPUT)/hi2isis_BG1.cub \
	  to=$(OUTPUT)/BG1_Ancillary.txt \
	  name="HiRISE Ancillary" >/dev/null;

	  tabledump from=$(OUTPUT)/hi2isis_IR0.cub \
	  to=$(OUTPUT)/IR0_CalibrationImage.txt \
	  name="HiRISE Calibration Image" >/dev/null;
	  tabledump from=$(OUTPUT)/hi2isis_IR0.cub \
	  to=$(OUTPUT)/IR0_CalibrationAncillary.txt \
	  name="HiRISE Calibration Ancillary" >/dev/null;
	  tabledump from=$(OUTPUT)/hi2isis_IR0.cub \
	  to=$(OUTPUT)/IR0_Ancillary.txt \
	  name="HiRISE Ancillary" >/dev/null;

	  tabledump from=$(OUTPUT)/hi2isis_IR1.cub \
	  to=$(OUTPUT)/IR1_CalibrationImage.txt \
	  name="HiRISE Calibration Image" >/dev/null;
	  tabledump from=$(OUTPUT)/hi2isis_IR1.cub \
	  to=$(OUTPUT)/IR1_CalibrationAncillary.txt \
	  name="HiRISE Calibration Ancillary" >/dev/null;
	  tabledump from=$(OUTPUT)/hi2isis_IR1.cub \
	  to=$(OUTPUT)/IR1_Ancillary.txt \
	  name="HiRISE Ancillary" >/dev/null;

	  tabledump from=$(OUTPUT)/hi2isis_RED0.cub \
	  to=$(OUTPUT)/RED0_CalibrationImage.txt \
	  name="HiRISE Calibration Image" >/dev/null;
	  tabledump from=$(OUTPUT)/hi2isis_RED0.cub \
	  to=$(OUTPUT)/RED0_CalibrationAncillary.txt \
	  name="HiRISE Calibration Ancillary" >/dev/null;
	  tabledump from=$(OUTPUT)/hi2isis_RED0.cub \
	  to=$(OUTPUT)/RED0_Ancillary.txt \
	  name="HiRISE Ancillary" >/dev/null;

	  tabledump from=$(OUTPUT)/hi2isis_RED1.cub \
	  to=$(OUTPUT)/RED1_CalibrationImage.txt \
	  name="HiRISE Calibration Image" >/dev/null;
	  tabledump from=$(OUTPUT)/hi2isis_RED1.cub \
	  to=$(OUTPUT)/RED1_CalibrationAncillary.txt \
	  name="HiRISE Calibration Ancillary" >/dev/null;
	  tabledump from=$(OUTPUT)/hi2isis_RED1.cub \
	  to=$(OUTPUT)/RED1_Ancillary.txt \
	  name="HiRISE Ancillary" >/dev/null;
