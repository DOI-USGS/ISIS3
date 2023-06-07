- Action Name: Remove Old LOLA/GRAIL SPKs
- Start Date: 2020-02-18
- RFC PR: (empty until a PR is opened)
- Issue: [https://github.com/DOI-USGS/ISIS3/issues/3689](https://github.com/DOI-USGS/ISIS3/issues/3689) (Please comment here!)
- Author: Stuart Sides

# Summary
Remove old LOLA GRAIL SPK files from the LRO/kernels/spk directory (i.e., LRO*GRGM660*.bsp). This would leave
only the newest gravity model SPKs (i.e., LRO*GRGM900C*.BSP)

# Motivation
The most recent LOLA/GRAIL SPK files use an improved gravity model, GRGM900C, instead of the older gravity model GRGM660. Also, all new LOLA/GRAIL SPK files are being delivered using the new model and not the old model. 

We propose to remove the older files to:
* Reduce the amount of data in the ISIS data area
* Reduce possible confusion caused by having two sets of LOLA/GRAIL SPK files with different gravity models
* Any new processing should most likely use the most accurate SPK for LRO spacecraft position

# Proposed Solution / Explanation
Remove all LRO SPK SPICE files that use the GRGM660 gravity model, and remove them from the "SMITHED" quality designation in the kernel.????.db file.

Removing these files would remove the capability for the ISIS application 'spiceinit' to automatically pick up the older gravity model SPK files, and also remove the ability for the user to specify any of the older gravity model files as the SPK argument.

# Drawbacks
Any currently active projects that are using the older gravity model SPK files would need to be reinitialized with 'spiceinit'. This will cause changes to the output of "jigsaw", but the results should be better given the improved position data.

# Alternatives
  - Leave the older gravity model SPK files in the LRO/kernels/SPK directory.

# Unresolved Questions
  - None at present. This will be updated to include any unresolved questions when/if there are any.

# Future Possibilities
  - None at present.