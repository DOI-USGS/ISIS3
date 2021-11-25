{% extends "qube_base.tpl" %}

{% block instrument %}
{{ super() }}
SpacecraftClockStopCount = {{ infoGroup.SPACECRAFT_CLOCK_STOP_COUNT.Value }}
StopTime             = {% set stopTime=infoGroup.STOP_TIME.Value %}
                       {{ RemoveStartTimeZ(stopTime) }}
NativeStartTime      = {{ infoGroup.NATIVE_START_TIME.Value }}
NativeStopTime       = {{ infoGroup.NATIVE_STOP_TIME.Value }}
InterlineDelayDuration = {{ infoGroup.INTERLINE_DELAY_DURATION.Value }}
XOffset              = {{ infoGroup.X_OFFSET.Value }}
ZOffset              = {{ infoGroup.Z_OFFSET.Value }}
SwathWidth           = {{ infoGroup.SWATH_WIDTH.Value }}
SwathLength          = {{ infoGroup.SWATH_LENGTH.Value }}
Channel              = (IR, VIS)
ExposureDuration     = ({{ QUBE.EXPOSURE_DURATION.Value.0 }} <IR>, {{ QUBE.EXPOSURE_DURATION.Value.1 }} <VIS>)
GainMode             = ({{ QUBE.GAIN_MODE_ID.Value.0 }}, {{ QUBE.GAIN_MODE_ID.Value.1 }})
{% endblock %}

{% block archive %}
PowerStateFlag       = ({{ infoGroup.POWER_STATE_FLAG.Value.0 }}, {{ infoGroup.POWER_STATE_FLAG.Value.1 }})
SpectralSummingFlag  = {{ infoGroup.SPECTRAL_SUMMING_FLAG.Value }}
SpectralEditingFlag  = {{ infoGroup.SPECTRAL_EDITING_FLAG.Value }}
StarTracking         = {{ infoGroup.STAR_TRACKING.Value }}
SnapshotMode         = {{ infoGroup.SNAPSHOT_MODE.Value }}
SamplingMode         = ({{ QUBE.SAMPLING_MODE_ID.Value.0 }}, {{ QUBE.SAMPLING_MODE_ID.Value.1 }})
{% endblock%}
