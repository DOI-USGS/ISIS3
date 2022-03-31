{% extends "img_base.tpl" %}

{% block mapping %}
{{ super() }}
{% endblock %}


Group = BandBin
  FilterName   = BroadBand
  Center       = {{ CENTER_FILTER_WAVELENGTH.Value }} <ms>
  Width        = {{ BANDWIDTH.Value }} <ms>
End_Group


