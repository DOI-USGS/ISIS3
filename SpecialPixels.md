# Special Pixels

---

## Introduction to Special Pixels

---

### What are special pixels?

Special pixels are defined to distinguish valid pixels from non-valid pixels in ISIS3. This is very important to scientists who evaluate the density values, and draw conclusions about the physical properties of a scene.

### How are special pixels set?

There are two ways that [[DN|Digital Number (DN)]] values are set to special pixels in ISIS3: 

*  **Instrument Special Pixels**: These types of special pixels are set during the ingestion of the data into ISIS3 because the acquired measurement either exceeded the dynamic range of the sensor based on its inherent properties (such as sensitivity) and settings (such as gain) or the measurement wasn't collected due to an instrument or transmission error. 
  * *Low Instrument Saturation ([[LIS]])*: The sensor registered a value too low to be measured accurately (i.e. undersaturated).
  * *High Instrument Saturation ([[HIS]])*: The sensor registered a value too high to be measured accurately (i.e. oversaturated).
  * *No Data Collected ([[NULL]])*: The instrument did not collect a measurement due to an sensor malfunction (such as a damaged CCD element) or an error in transmission prevented the value from being recorded. 
* **Representation Special Pixels**: These types of special pixels are set due to being processed by ISIS3 applications that cause the DNs to fall outside the data range of the file, either due to settings defined by the user or the effects of the algorithms within the application. 
  * *Low Representation Saturation ([[LRS]])*: The resulting DN calculated by the application fall below the possible range of values (i.e. undersaturated).
  * *High Representation Saturation ([[HRS]])*: The resulting DN calculated by the application are higher than the possible range of values (i.e. oversaturated).
  * *Data Removed ([[NULL]])*: NULL values are set by an application when it removes data, such as during masking or geometric warping.

## Special Pixels in ISIS3

---

### How many special pixel values exist in ISIS3?

There are five special pixel values defined in the ISIS3 software system for 16-bit and 32-bit data type. Each of the five special pixels is denoted by a specific value that represents that specific type of invalid data. **Having one of these special value is the only way ISIS3 can tell that a given pixel is in fact a special pixel of a particular type.** The [[NULL]] special pixel identifies pixels where either the instrument failed to collect data, or data was removed or empty pixels created during processing. The [[LRS|LRS (Low Representation Saturation)]] and [[HRS|HRS (High Representation Saturation)]] represent saturated pixels that fall outside the minimum and maximum valid range of the file respectively. The [[LIS|LIS (Low Instrument Saturation)]] values represent pixels that fall below the lowest value the instrument can accurately record. The [[HIS|HIS (High Instrument Saturation)]] values represent pixels that exceed the highest value the instrument can accurately record. The table to the right shows each of the 5 special pixel types and their corresponding numerical representations. Note that 8-bit data can only has two special pixel types, represented by values of 0 ([[NULL]], [[LIS]], [[LRS]]) and 255 ([[HIS]] and [[HRS]]). Thus 8-bit data does not distinguish between [[NULL]], [[LIS]], and [[LRS]] pixels nor between [[HIS]] and [[HRS]] pixels. 

**Special Pixels in ISIS3** Visual refers to the apparent color of a special pixel in an ISIS3 display application, such as **qview**. 

| **Special Pixel Name** | **32-bit** \* | **16-bit** | **8-bit** | **Visual** | 
|---|---|---|---|---|
| NULL | -3.40282e+38 | -32768 | 0 | black |
| Low Representation Saturation (LRS) | -3.40282e+38 | -32767 | 0 | black |
| Low Instrument Saturation (LIS) | -3.40282e+38 | -32766 | 0 | black |
| High Instrument Saturation (HIS) | -3.40282e+38 | -32765 | 255 | white |
| High Representation Saturation (HRS) | -3.40282e+38 | -32764 | 255 | white |

* \* The 32-bit values representing different special pixels do differ just like those representing the 16-bit valus. However, the variations are on the last digit, and thus are not visible in this display since it does not show all 38 digits.

### Are the special pixel values propagated between different bit types?

The special pixel values are converted without loss of information between 16-bit and 32-bit data. See the chart above to see what the values are set to after conversion from one bit type to the other. For example, if a pixel in 16-bit data with a DN of -32766 (thus representing an [[LIS]] pixel) is converted to 32-bit data, its DN value will be changed to that value representing an LIS pixel in 32-bit format. If the image is converted from 16-bit or 32-bit to an 8-bit data type, then all the [[LIS]], [[LRS]], and [[NULL]] special pixel have their values set to 0, and all [[HIS]] and [[HRS]] special pixels have their values set to 255. Note that this conversion results in a loss of information, as it is no longer possible to distinguish between [[LIS]], [[LRS]], and [[NULL]] pixels since they all have the same pixel value of 0. Similarly, converting from 16-bit or 32-bit to 8-bit will make it impossible to distinguish between [[HIS]] and [[HRS]] pixels. Why? 

### Is the NULL special pixel defined as 0 for all bit types?

No. The [[DN]] value 0 is defined as [[NULL]] only in an 8-bit file. For the 16-bit and 32-bit data, 0 is a valid [[DN]]!

## Interactive Special Pixel Demonstration

---

{{isisdemo(isis-special-pixels)}}

## Working with Special Pixels

---

Q: *How do I find out if there are any special pixels in my file?*
A: Statistics: **hist** & **stats**



There are two programs [**stats**](https://isis.astrogeology.usgs.gov/Application/presentation/Tabbed/stats/stats.html) and [**hist**](https://isis.astrogeology.usgs.gov/Application/presentation/Tabbed/hist/hist.html) that will provide statistics about the pixels of an image. The output from stats looks like:

~~~
Group = Results
    From                    = Intro2isis.cub
    Band                    = 1
    Average                 = 61.770642254011
    StandardDeviation       = 8.0295403784653
    Variance                = 64.473518689404
    Median                  = 62.0
    Mode                    = 66.0
    Skew                    = -0.085692730285373
    Minimum                 = 2.0
    Maximum                 = 252.0
    Sum                     = 76029592.0
    TotalPixels             = 1271424
    ValidPixels             = 1230837
    OverValidMaximumPixels  = 0
    UnderValidMinimumPixels = 0
    NullPixels              = 40587
    LisPixels               = 0
    LrsPixels               = 0
    HisPixels               = 0
    HrsPixels               = 0
End_Group
~~~

**View your image: [qview](http://isis.astrogeology.usgs.gov/Application/presentation/Tabbed/qview/qview.html)**

You can also display the image on a monitor using the program [**qview**](http://isis.astrogeology.usgs.gov/Application/presentation/Tabbed/qview/qview.html), and position the cursor over a very dark pixel. The [[DN]] displayed on the screen will either be [[NULL]], [[LIS]], [[LRS]], or a very low [[DN]] value. The [[HIS]] and [[HRS]] pixels will be displayed as white pixels on a monitor.

## Related ISIS3 Applications

---

* [**hist**](https://isis.astrogeology.usgs.gov/Application/presentation/Tabbed/hist/hist.html)
* [**stats**](https://isis.astrogeology.usgs.gov/Application/presentation/Tabbed/stats/stats.html)
* [**qview**](http://isis.astrogeology.usgs.gov/Application/presentation/Tabbed/qview/qview.html)
* [**isis2ascii**](http://isis.astrogeology.usgs.gov/Application/presentation/Tabbed/isis2ascii/isis2ascii.html)