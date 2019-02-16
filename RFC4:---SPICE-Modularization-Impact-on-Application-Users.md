- Feature/Process Name: SPICE Modularization Impact on Application Users
- Start Date: 2.15.19
- RFC PR: (empty until a PR is opened)
- Issue: (link or create and link and associated issue for discussion)
- Author: Laura

<!-- This is a comment block that is not visible. We provide some instructions in here. When submitting an RFC please copy this template into a new wiki page titled RFC#:Title, where the number is the next incrementing number. If you would like to submit an RFC, but are unable to edit the wiki, please open an issue and we will assist you in getting your RFC posted. Please fill in, to the largest extent possible, the template below describing your RFC. After that, be active on the associated issue and we can move the RFC through the process.-->

# Summary
The recently released [RFC3](https://github.com/USGS-Astrogeology/ISIS3/wiki/RFC-3:-Spice-Modularization) focuses on the technical impact of the proposed changes ISIS3 spice, spiceposition, and spicerotation classes and the associated breaking API changes. This RFC piggybacks on [RFC3](https://github.com/USGS-Astrogeology/ISIS3/wiki/RFC-3:-Spice-Modularization) with a focus on the impact to ISIS3 application consumers. We assume that most users are application users, whether pipelining apps (`pds2isis->spiceinit->cam2map->isis2std`), working interactively (`qview` or `cnet*` for example), or manually executing any of the 300+ independent GUI or command line tools. If those use cases resonate with you, this is RFC to checkout first. [RFC3](https://github.com/USGS-Astrogeology/ISIS3/wiki/RFC-3:-Spice-Modularization) is more technical in nature.

# Motivation
ISIS3 is a powerful tool for getting access to the information necessary to use a camera model. 99.9% of the time, that information comes from NAIF SPICE data. When you run many (most) of the applications after `spiceinit`, chances are, you are making use of pieces of ISIS3 that work with SPICE data. Most of time, the complexity of this  is well hidden from you. In some cases though, you might actually want access to said information and when that happens, we struggle to provide you with a clean way to do this. Our motivation for [RFC3](https://github.com/USGS-Astrogeology/ISIS3/wiki/RFC-3:-Spice-Modularization) is two fold: (1) we, developers, need to reduce the implementation complexity of how we identify, open, store, query, and manipulate ephemeris information for a whole host of technical reasons (if those are of interest, see [RFC3](https://github.com/USGS-Astrogeology/ISIS3/wiki/RFC-3:-Spice-Modularization)) and (2) we would like to provide an easier to use, interrogate and extend layer for those users interested in doing work or research with the spacecraft ephemeris information. 

# Proposed Solution / Explanation
The technical implementation is well described in [RFC3](https://github.com/USGS-Astrogeology/ISIS3/wiki/RFC-3:-Spice-Modularization). Herein, I would like to focus on the impact of the proposed changes. To get it out of the way as rapidly as possible: **The proposed change would be API breaking. Therefore, we would be required to increment the major version number from 3 (ISIS3) to 4 (ISIS4). See [RFC2](https://github.com/USGS-Astrogeology/ISIS3/wiki/RFC2:-Release-Process#versioning) for why incrementing the major version number is required.** The remainder of this section discusses the rationale and implications of this change.

First, some background. ASC has recently adopted the [semantic versioning standard](https://semver.org) which states that any API breaking changes need to result in a major version increase. The way in which most people use ISIS3 means that our applications and all of the arguments that are used when those applications are run are our public facing API. For example, `spiceinit` currently accepts `FROM=`, `WEB`, `ATTACH=`, etc. arguments, entered either via the command line or in the `spiceinit` GUI. These arguments are the API and we have adopted a policy whereby any change to those arguments that is not backwards compatible (more on this below) requires that we increment the version number.

Why on earth would we want to do this? We have heard from our users time and again about the frustration when we make a breaking change to the application arguments that then requires said users to update scripts, pipelines, hit our documentation again for assistance, etc. Instead of 'stealthily' rolling out these changes, we want to (1) make API changes the big deal that they should be and (2) loudly announce the change so that no one is taken off guard (both as these RFCs and when you download ISIS4).

On compatibility: **The proposed change would be backward, but not forward compatible.** What does that mean? 

Data created (`spiceinit`) or adjusted (`jigsaw`, deltack, etc.) using ISIS3 would be usable in ISIS4. ISIS4 is backwards compatible with ISIS3. Data created (`spiceinit`) or adjusted (`jigsaw`, deltack, etc.) using ISIS4 would not be readable or usable (forward compatible) in ISIS3. [RFC3](https://github.com/USGS-Astrogeology/ISIS3/wiki/RFC-3:-Spice-Modularization) is not proposing to deprecate any applications between ISIS3 and ISIS4. The proposed change is intentionally being kept away from the application API layer as much as possible to both normalize this type of semantic versioning with our user base and in order to provide us with a tractable problem scope (in RFC3).

## Anticipated application level changes
  - `spiceinit` - The attach argument will be removed. SPICE Data will now always be attached.
  - Cubes created in ISIS3 with kernels selected but no SPICE attached will need to be re-spiceinit'd in order to query the kernels and attach the SPICE data.

# Drawbacks
Change is scary. [RFC3](https://github.com/USGS-Astrogeology/ISIS3/wiki/RFC-3:-Spice-Modularization) describes the technical hurdles we anticipate with implementation. We also see a hurdle in communicating our goals and rationale to our non-developer user community. 

# Alternatives
  - We have explored maintaining both backwards and forwards compatibility. The cost of forwards compatibility is non-trivial.
  - Support Cubes creates in ISIS3 with selected kernels, but no attached SPICE data by loading kernels and querying when the cube is loaded. This would require additional interaction between the ALE IO code and increased complexity in the ISIS camera model hierarchy. Providing a one time update utility to handle these types of cubes will provide almost as much utility at greatly reduced complexity and maintenance overhead.

# Unresolved Questions
  - We (ASC and the ISIS3 development team) continue to seek the best methods to communicate potential changes to ISIS3.

# Future Possibilities
  - Rewrite spkwriter and ckwriter functionality to be a part of the [ALE](https://github.com/USGS-Astrogeology/ale) library. This will allow for users to generate SPICE C and SP kernels either through ISIS4 or the more light weight ALE. This also creates a path for generating SPICE C and SP kernels using our [Community Sensor Model implementation](https://github.com/USGS-Astrogeology/usgscsm).