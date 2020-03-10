# Build ISIS
To get started, first check out our [Developing ISIS3 with CMake](https://github.com/USGS-Astrogeology/ISIS3/wiki/Developing-ISIS3-with-cmake) page to setup a clone of ISIS and an Anaconda environment to build with. After following the steps on the page, you should have a local build of ISIS to start developing with. 

# Pick an Issue
Next, you are ready to pick an issue to start working on. Head over to the [issues page](https://github.com/USGS-Astrogeology/ISIS3/issues), where you will find bugs, questions, and feature requests. You will want to filter these issues with our “good first issue” label. To do this, click on the “labels” dropdown and click on the purple “good first issue” label. You may also go to this [link](https://github.com/USGS-Astrogeology/ISIS3/issues?q=is%3Aopen+is%3Aissue+label%3A%22good+first+issue%22). This will show you all of the open issues that will be a good place to start developing. Pick an issue that you may want to work on, click on the issue title to go to its page,  and assign yourself to that issue by clicking on “assign yourself” under the “Assignees” section to the right of the issue description. Now, you are ready to start developing.

# Document Your Changes
Once you have completed the ticket, i.e., by fixing an issue or adding a feature, make sure you have updated any documentation. This includes adding a history entry in the application’s XML file or the object’s header file that you changed. For example, if you made changes to the ISIS program `spiceinit`, you will add a history entry in the `spiceinit.xml` file. If you needed to add a class, make sure to follow our [formatting documentation page](https://github.com/USGS-Astrogeology/ISIS3/wiki/ISIS3-Class-Documentation-Requirements-Using-Doxygen-tags) for formatting documentation. 

# Test Your Changes
You will also need to test the code, update any tests that are now failing because of the changes, or add new tests that cover your changes. Check out our [running tests page](https://github.com/USGS-Astrogeology/ISIS3/wiki/Developing-ISIS3-with-cmake#running-tests) to run any tests associated with your changes. Check out our [writing test page](https://github.com/USGS-Astrogeology/ISIS3/wiki/Writing-ISIS3-Tests-Using-Gtest-and-Ctest) to learn how to write a test using Gtest and Ctest. 

# Create a Pull Request
Next, commit your changes to your local branch with the command 
`git commit -m “<message>”` where <message> is your commit message. Then, push up your changes to your fork 
`git push origin <branch_name>` where `<branch_name>` is the name of your local branch you made your changes on. Finally, head over to your fork, click on the “Branch” dropdown, select the branch `<branch_name>` from the dropdown, and click “New Pull Request”. Give your PR a descriptive but brief title and fill out the description skeleton. After submitting your PR, wait for someone to review it, and make any necessary changes.
