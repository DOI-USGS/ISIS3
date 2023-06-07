- Feature/Process Name: Import Export Program Redesign
- Start Date: August 23, 2021
- RFC PR: forthcoming
- Issue: [#4606](https://github.com/DOI-USGS/ISIS3/issues/4606)
- Author: Stuart Sides

# Summary

ISIS has individual programs for importing and exporting images. Some programs are
very specific for example, lronac2isis will convert a PDS3 formatted LRO NAC image 
into an ISIS cube with all the required labels to instantiate the nac camer model,
hirdrgen will convert a processed HiRISE ISIS cube to a PDS3 formatted image.
There are approximately 70 specialized import/export programs. 

This RFC proposes a redesign of most of those applications into a single import and a single export
application.

# Motivation

Many users of the ISIS software have expressed frustration identifying which ISIS program to use
to import or export image data to/from ISIS. This issue is further complicated with generic ISIS 
programs (e.g., pds2isis and isis2pds) that are available to convert the image data and some
label information (e.g., projection information), but do not convert instrument
specific information such as timing or SPICE information necessary to work with an instruments camera
model. For import, pds2isis allows generalized image processing applications to run such as qview and lowpass.

The individual import/export programs encapsulate most of the instrument specific software, and the 
generalized ISIS library contains the image data IO software. Most of the instrument specific software
is used to add and modify information in the cube label. 

# Proposed Solution / Explanation

Since cube labels in ISIS are PVL text, PDS3 image labels are PVL text, and PDS4 labels are XML text, we
propose to have a single application that uses a template engine library to convert PDS4 and PDS3
labels to ISIS labels (import) and another single application to convert ISIS labels to 
PDS3 and PDS4 labels (export) and continue to use the existing I/O functionality in ISIS to read and write
the image data.

# Prof of Concept

We have identified a reasonably mature C++ template engine library called [Inja](https://pantor.github.io/inja/).
Using Inja we have successfully reproduced the TGO CaSSIS import and export programs. There are two new programs in ISIS
"isisimport" and "isisexport" that use Inja templates to convert labels. These programs require an input template that conforms to the desired output 
(i.e., ISIS cube, PDS3 PVL, PDS4 XML) with embedded Inja syntax to handle the logic that currently exists in the instrument specific programs.

# Benifits

- Users will be able to use a single program to import and a single program to export most image data to/from ISIS
- Users will be able to modify templates without recompiling ISIS
- Instrument teams will be able to prototype PDS4 archive labels quickly without recompiling ISIS 
- Label information that is standard across multiple instruments can reuse a single sub-template
- Maintenance of the mature templates will be straight forward
- Instrument teams will be able to simplify their pipelines by removing multiple calls to "editlab"

# Drawbacks

- Not all import/export programs will be able to be converted. For example "raw2isis" does not expect any labels.
  - The necessary parameters could be supplied through the ISIS user interface, but it would add a substantial number of arguments to the program
- Developers and interested users will need to learn the Inja template syntax
- Inja templates for PDS4 XML will be large and complex
- The number of programs to be converted will take a significant effort
- Users and instrument teams will need to change existing pipelines to use the new programs

# Alternatives

- Continue to use dedicated programs for each instrument
- Explore using gdal instead of ISIS
  - gdal does not currently produce ISIS labels with the necessary information to instantiate camera models
  - gdal templates are simple find-and-replase
- Explore using gdal inside of ISIS

# Unresolved issues

- Programs that currently work with binary data other than the main image data may continue to require specialize C++ code to produce fully functional ISIS cubes. For example hrsc2isis processes line prefix data and produces a line exposure table required by the camera model.
  - A possible solution would be to define a plugin interface to be used to process the non-text based information
  - A possible solution would be to convert the binary data to text
- Programs that are no longer being used by an active instrument team should be considered for deprecation instead of converting them.

# Design

## Use Cases

- I have a TGO CaSSIS PDS4 archive image and want to run ISIS programs on it with a CaSSIS camera model.
- I have a Viking I PDS3 archive image and want to run ISIS programs on it with a camera model.
- Other mission/instrument combinations are similar to the two above
- I have a New Horizons LORRI FITS formatted image ...

## Considerations 

- Need to limit if not eliminate references to the missions and instruments in the ISIS C++ code. This should be possible by using a preprocessing template to determine what main template should be used. 
- Consider plugins for special cases of binary data  
- Inja (C++) is not as powerful as Jinja (Python)
- How does the engine access the data from multiple sources? 
- Will need to extend the existing ISIS import/export processes 
- Use PDS tools to verify the accuracy of the output PDS4 products 
- External information (user added information) not available in the labels or elsewhere in the input file(s)
  - Producer ID 
  - PDS4 tags/IDs 

## Formats to be considered

- PDS3 label 
- PDS4 label 
- FIT label 
- ISIS2 cubes
- Instrument specific versions

### Import program flow

- Use a template to build a template file name from the input label.
  - Example: NewHorizonsLorriFit.tpl
- Allow additional input data from the user
  - Label information that is not represented in the input label
- Process the template file using Inja
- Write the Cube label to the Cube file
- Extract the file names (.IMG, .TBL, ...), byte counts, starting bytes, data types, ... from the input label
- Call the ISIS library ProcessImport
- Execute any needed plugins for binary data

### Export program flow

- Use a template to build a template file name from the ISIS cube
  - Example: TgoCassisRdrPds4.tpl
- Add ISIS Original label to the Inja data structure
- Add users supplied PVL, XML, CSV, ... to the Inja data structure
- Process the derived template with Inja
- Call the ISIS library ProcessExport
- Export any additional ISIS tabels, BLOBs, ...
