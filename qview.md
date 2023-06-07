# Description
This program will display cubes and allow for interactive analysis.

## Tools
There are several analysis tools located on the right side vertical toolbar. Activating one of these tools will
add controls specific to that tool to the top toolbar. Only one tool can be active at a time, but some are persistent even when not active. For example, the zoom tool's shortcut keystrokes remain available when other tools are active.

<!--- INSERT IMAGE OF THE SIDE TOOLBAR. --->

###
Band Selection

###
Zoom

###
Pan

### 
Stretch

###
Find

###
Image Edit

###
Measure [[https://github.com/DOI-USGS/ISIS3/blob/dev/isis/appdata/images/icons/measure.png|alt=measuretool]]

Displays user defined measurements of area, length, and angle on an image. There are two controls in the top toolbar for the measure tool. A combo box to choose the what shape is drawn, and a combo box to define the units of the values displayed (e.g., meters, kilometers, or pixels).

<!--- INSERT SCREEN IMAGE HERE. consider highlighting the two controls --->

The mouse is used to draw circles, ellipses, rectangles, rotated rectangles, polygons, lines, segmented lines and angles on a displayed image. The area, length, or angle is then displayed. The units of the measurement can be controlled with second combo box.

* Circle - To draw a circle use the mouse to define a square area on the image by clicking and holding in one corner of the square then dragging to the opposite corner and letting up. The circle will be drawn and the area of the circle will be displayed as the mouse moves. The units can be changed at any time.

* Line - To draw a line click and hold where you want the line to begin and drag to where you want the line to end then let up. The distance will be displayed as the mouse moves. There are four choices for what units the distance is displayed in:
    - Meters (m) - Displays the distance between the endpoints of the line in meters. This requires a spiceinit'ed or projected cube. The distance is calculated by converting the endpoints to their corresponding latitudes and longitudes and then using a great circle algorithm to calculate the distance between them. 
    - Kilometers (km) - Displays the distance between the endpoints of the line in kilometers. This requires a spiceinit'ed or projected cube. The distance is calculated in the same manner as for "meters" and converted to kilometers.
    - Pixels - Display the distance between the endpoints of the line in pixels. This option is always available for any cube. The distance is calculated using the Pythagorean theorem.
    - Planer Kilometers - Displays the distance between the endpoints of the line in kilometers. This option is only available if there is a camera model available for the image and at least one of the points is on the surface of the target body defined in the cube label. The distance is calculated using the angle between the right ascension and declination of the two endpoints, and the slant range distance from the spacecraft to the point on the target surface. An isosceles triangle is assumed between the first point, the spacecraft, and the second point. The reported distance is the length of the base of the triangle. The equation used for this calculation is below. dec1, dec2, RA1, and RA2 refer to the right ascension and declination of the starting and ending point of the line, respectively, and d_slant is the slant range of the point on the target body, which can be obtained from the camera model.

![planareq]

![planareq1]

An example of using the Measure Tool to measure the Planar Kilometer distance of a volcanic plume above Io is shown below: 

![image](https://user-images.githubusercontent.com/22879031/99570570-5d75de80-298f-11eb-977c-ddfcc97d7876.png)

###
Sun Shadow

###
Nomenclature

###
Spatial Plot

Display and analyze cubes


###
Spectral Plot

###
Scatter Plot

###
Histogram

###
Statistics

###
Stereo

####
Match

[planareq]: http://chart.apis.google.com/chart?cht=tx&chl=\theta=\arccos{(\sin{(dec_1)}\sin{(dec_2)}+\cos{(dec_1)}\cos{(dec_2)}\cos{(RA_1-RA_2)})}

[planareq1]: http://chart.apis.google.com/chart?cht=tx&chl=d_{mesaured}=d_{slant}2\sin{\frac{\theta}{2}}
