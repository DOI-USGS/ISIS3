<p align="center">
  <img src="isis/src/docsys/assets/img/image-source-files/ISIS_Logo.svg" alt="ISIS" width=200> 
</p>

# ISIS

[![Anaconda-Server Badge](https://anaconda.org/usgs-astrogeology/isis3/badges/version.svg)](https://anaconda.org/usgs-astrogeology/isis3)
[![Anaconda-Server Badge](https://anaconda.org/usgs-astrogeology/isis/badges/version.svg)](https://anaconda.org/usgs-astrogeology/isis)


## Quick Reference

To start using ISIS, see:

- [Installing ISIS](https://astrogeology.usgs.gov/docs/how-to-guides/environment-setup-and-maintenance/installing-isis-via-anaconda/)
    - Distubuted with conda, compatible with Unix and Mac.
- [Intro to ISIS](https://astrogeology.usgs.gov/docs/getting-started/using-isis-first-steps/introduction-to-isis/)
    - A good primer, at the end there's an example you can try!
- [Setting Up the ISIS Data Area](https://astrogeology.usgs.gov/docs/how-to-guides/environment-setup-and-maintenance/isis-data-area/)
    - You'll need this to process images on your own.

For more info, see:

- [Tutorials and Guides](https://astrogeology.usgs.gov/docs/)
- [FAQ](https://astrogeology.usgs.gov/docs/getting-started/using-isis-first-steps/isis-faq/)
- [ISIS Application Manuals](https://isis.astrogeology.usgs.gov)

For Development and Contributing, see:

- [Building ISIS](https://astrogeology.usgs.gov/docs/how-to-guides/isis-developer-guides/developing-isis3-with-cmake/)
- [Contributing](https://astrogeology.usgs.gov/docs/how-to-guides/isis-developer-guides/contributing-to-isis/)

## In this README:

- [Requests for Comment](#Requests-for-Comment)
- [Citing ISIS](#Citing-ISIS)
- [System Requirements](#system-requirements)
- [Semantic Versioning](#Semantic-Versioning)


## Requests for Comment
The ISIS project uses a Request for Comment (RFC) model where major changes to the code, data area, or distribution are proposed, discussed, and potentially adopted.  All contributors and users are welcome to review and comment on open RFCs.

See this repo's [discussion page](https://github.com/DOI-USGS/ISIS3/discussions?discussions_q=is%3Aopen+label%3ARFC) for open RFCs.


## Citing ISIS

If you use ISIS in your research, please cite the specific version you used:

### Latest Release 

**APA Format:**
```
USGS Astrogeology Science Center. (2026). Integrated Software for Imagers and Spectrometers (ISIS) (Version 9.0) [Computer software]. U.S. Geological Survey. https://doi.org/10.5066/P14RZST9
```

**BibTeX Format:**
```bibtex
@software{ISIS9.0.0,
  author = {{USGS Astrogeology Science Center}},
  title = {{Integrated Software for Imagers and Spectrometers (ISIS) version 9.0.0}},
  year = {2026},
  publisher = {{U.S. Geological Survey}},
  version = {9.0},
  doi = {DOI-10.5066/P14RZST9},
  url = {{https://doi.org/10.5066/P14RZST9}}
}
```

Use "Cite this repository" button on dev branch for citation information for dev builds. 

### Citing Previous Versions

To cite a specific earlier version, visit our [GitHub Releases](https://github.com/DOI-USGS/ISIS3/releases) page and click on the version you used. Each release starting with 10.0.0 has a "Cite this repository" button with version-specific citation formats.

Pre-10.0.0 have DOIs in their release page which can be used to generate a citation.

### Machine-Readable Citation

For automated citation tools, see our [CITATION.cff](CITATION.cff) file. This file is automatically recognized by GitHub, Zotero, and other citation managers.

### Questions?

If you have questions about citing ISIS, please [open an issue](https://github.com/DOI-USGS/ISIS3/issues/new).
## System Requirements

ISIS is supported on these UNIX variants (and may work on others, though unsupported):

-   Ubuntu 18.04 LTS
-   macOS 10.13.6 High Sierra
-   Fedora 28
-   CentOS 7.2

[//]: # ( These are all past their vendors' standard support, most EOL:         )
[//]: # ( Ubuntu 18.04LTS            Released 2018.04, ESS 2023.05, EOL 2028.04 )
[//]: # ( macOS 10.13.6 High Sierra  Released 2017.09,              EOL 2020.11 )
[//]: # ( Fedora 28                  Released 2018.05,              EOL 2019.05 )
[//]: # ( CentOS 7.2                 Released 2015.12,              EOL 2024.06 )

ISIS is not supported on Windows, but 
[using WSL](https://planetarygis.blogspot.com/2024/02/isis-and-asp-on-windows-11-wsl-take-3.html) 
may be possible.

#### Architecture Support

- 64-bit x86 processors: Supported. 
- Apple Silicon: In Development.

#### Storage Space Required

- 2.5 GB for ISIS binaries
- 10 GB up to multiple TB for mission data and processing large images


## Semantic Versioning

Versions of ISIS now use a Major.Minor.Patch scheme (e.g., 8.3.0), detailed in [RFC 2](https://github.com/DOI-USGS/ISIS3/wiki/RFC2:-Release-Process#terms). 

| Major | Minor | Patch |
|-------|-------|-------|
| 8     | .3    | .0    |


- **Patch Releases** (i.e. 8.0.2 → 8.0.3) fix a bug, but don't make breaking changes or add features.

- **Minor Releases** (i.e. 8.2.0 → 8.3.0) add features, but no breaking changes.

- **Major Releases** (i.e. 8.3.0 → 9.0.0) add a breaking change.

#### Breaking Changes

A breaking change alters API signatures, existing arguments to ISIS apps, or output.  Anything that could break backwards compatibility with a script is considered breaking*.

Additions (i.e, an added optional argument, and added column in a .csv file) aren't considered breaking, but changes to existing output or input (i.e, changing an existing argment, changing the title of an existing .csv output column) are breaking.

> *.txt files, or output meant only for human readers, are excluded from ISIS's definition of a breaking change.

Contributors must make sure that breaking changes are well-identified.  Breaking changes require input, discussion, and approval from the community before they can be adopted into ISIS.

#### Upgrade considerations

Most users can safely upgrade to Minor and Patch Versions, but should be more cautious about a Major upgrade, which may introduce changes that could alter their workflow.  You can reference the [Changelog](https://github.com/DOI-USGS/ISIS3/blob/dev/CHANGELOG.md) for more specific information on the changes.

### ISIS Release Cadence (LTS)
ISIS has a Long Term Support (LTS) model ([RFC8](https://github.com/DOI-USGS/ISIS3/discussions/4691), [RFC14](https://github.com/DOI-USGS/ISIS3/discussions/5731)). This assumes that users will update at ***each LTS release*** (supported for 18 months), or use more frequent interim/dev releases with shorter-term support.
