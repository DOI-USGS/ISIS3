SPICE kernels are required for the use of most ISIS applications. ISIS provides SPICE kernels for each mission
in the ISISDATA area . For information on how to download the ISISDATA area, see: [Download Mission Data](https://github.com/DOI-USGS/ISIS3#mission-specific-data-downloads)

ISIS provides ongoing automated updates of SPICE kernels for some of its supported missions by checking for new mission kernels, and if available, distributing them via the ISISDATA area available from the rsync server. These supported missions are listed in the table below, alongside their ISIS and Mission update cadences. ‘ISIS Update’ refers to how often ISIS looks for and then distributes new SPICE kernel updates and ‘Mission Update’ refers to how often the mission typically provides updated kernels. If the mission update cadence is unknown, please contact the mission for more information. To receive newly updated kernels, it's necessary to re-run the rsync command described in 
[Download Mission Data](https://github.com/DOI-USGS/ISIS3#mission-specific-data-downloads).

Missions that are not listed in the table below are provided with a static set of SPICE kernels that are not automatically updated. These are occasionally updated by hand. If you would like to request that the kernels provided with ISIS for one of these missions be updated, please submit a ticket at  [new issue](https://github.com/DOI-USGS/ISIS3/issues/new/choose).

| Mission     | ISIS Update Cadence | Mission Update Cadence |
| ----------- | ------------------- | ----------------------
| Cassini     |  daily              | No kernel updates are planned at this time |
| CaSSIS      |  daily              | 1-3 times a week |
| Dawn        |  every 12 hours     | Planned update for Vesta SPC shape model and corresponding SPICE kernels in early 2022. |
| Hayabusa2   |  daily              | No kernel updates are planned at this time |
| Juno        |  daily              | Unknown |
| LRO         |  daily              | daily   |
| MEX         |  once a month       | V2 release date TBD. |
| MRO         |  daily              | CKs daily/weekly; SPKs semi-weekly; SCLKs, FK and IK updates, etc. - as needed |
| Odyssey     |  daily              | CKs – daily; Merged CKs – quarterly (month after quarter start); SPKs – weekly on Thursdays; Merged SPKs - quarterly (month after quarter start); SCLKs - monthly |

If you would like to receive notification from NAIF when kernels are publicly released, please sign up with [SPICE Announce](https://naif.jpl.nasa.gov/mailman/listinfo/spice_announce). You will also receive announcements from NAIF regarding new software, bug fixes, training opportunities and similar items. 