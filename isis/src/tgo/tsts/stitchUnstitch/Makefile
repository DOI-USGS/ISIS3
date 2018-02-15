APPNAME =

include $(ISISROOT)/make/isismake.tsts

commands:
	tgocassis2isis $(TSTARGS)                                                          \
            from=$(INPUT)/CAS-MCO-2016-11-26T22.50.27.381-PAN-00005-B1.xml           \
            to=$(OUTPUT)/panframelet.cub > /dev/null;

	spiceinit $(TSTARGS)                                                               \
            from=$(OUTPUT)/panframelet.cub                                           \
            ckp=t spkp=t > /dev/null;

	tgocassis2isis $(TSTARGS)                                                          \
            from=$(INPUT)/CAS-MCO-2016-11-26T22.50.27.381-RED-01005-B1.xml           \
            to=$(OUTPUT)/redframelet.cub > /dev/null;

	spiceinit $(TSTARGS)                                                               \
            from=$(OUTPUT)/redframelet.cub                                           \
            ckp=t spkp=t > /dev/null;

	tgocassis2isis $(TSTARGS)                                                          \
            from=$(INPUT)/CAS-MCO-2016-11-26T22.50.27.381-BLU-03005-B1.xml           \
            to=$(OUTPUT)/bluframelet.cub > /dev/null;

	spiceinit $(TSTARGS)                                                               \
            from=$(OUTPUT)/bluframelet.cub                                           \
            ckp=t spkp=t > /dev/null;

	tgocassis2isis $(TSTARGS)                                                          \
            from=$(INPUT)/CAS-MCO-2016-11-26T22.50.27.381-NIR-02005-B1.xml           \
            to=$(OUTPUT)/nirframelet.cub > /dev/null;

	spiceinit $(TSTARGS)                                                               \
            from=$(OUTPUT)/nirframelet.cub                                           \
            ckp=t spkp=t > /dev/null;

	tgocassisstitch $(TSTARGS)                                                         \
            fromlist=$(INPUT)/cubelist.lis                                           \
            outputprefix=$(OUTPUT)/"stitched" > /dev/null;

	tgocassisunstitch $(TSTARGS)                                                       \
            from=$(OUTPUT)/stitched-2016-11-26T22:50:27.381.cub                      \
            outputprefix=$(OUTPUT)/"unstitched" > /dev/null;

	cubediff $(TSTARGS)                                                                \
            from=$(OUTPUT)/nirframelet.cub                                           \
            from2=$(OUTPUT)/unstitched_NIR.cub > /dev/null;

	cubediff $(TSTARGS)                                                                \
            from=$(OUTPUT)/redframelet.cub                                           \
            from2=$(OUTPUT)/unstitched_RED.cub > /dev/null;

	cubediff $(TSTARGS)                                                                \
            from=$(OUTPUT)/bluframelet.cub                                           \
            from2=$(OUTPUT)/unstitched_BLU.cub > /dev/null;

	cubediff $(TSTARGS)                                                                \
            from=$(OUTPUT)/panframelet.cub                                           \
            from2=$(OUTPUT)/unstitched_PAN.cub > /dev/null;

	tgocassisstitch $(TSTARGS)                                                         \
            fromlist=$(INPUT)/cubelist2.lis                                          \
            outputprefix=$(OUTPUT)/"stitched2" > /dev/null;

	cubediff $(TSTARGS)                                                                \
            from=$(OUTPUT)/stitched-2016-11-26T22:50:27.381.cub                      \
            from2=$(OUTPUT)/stitched2-2016-11-26T22:50:27.381.cub > /dev/null;
            
	$(RM) $(OUTPUT)/unstitched.lis;
