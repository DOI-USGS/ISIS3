# Docker Container Specs

To specify a container for use in a Jenkins pipeline, label your agent with one of the following: 

* isis-fedora-25
* isis-ubuntu-1804
* isis-debian-9
* isis-centos-7

The labels above tell the Jenkins server that you want to create your agent using a USGS-Astrogeology Docker image. We currently maintain containers specs for building isis on the systems listed above. Their Dockerfiles can be found here: [link](https://github.com/USGS-Astrogeology/docker_linux_isisdeps/tree/master/jenkins)

These files are automatically pulled and built for DockerHub whenever new changes are made. Their images can be found here: [link](https://hub.docker.com/r/usgsastro/docker_linux_isisdeps/)

These images are built with some system library dependencies of ISIS and an install of miniconda.

# Jenkins Pipeline

Most of the build process is specified in a groovy script: https://github.com/USGS-Astrogeology/ISIS3/blob/dev/Jenkinsfile

Refer to this for syntax: https://jenkins.io/doc/book/pipeline/syntax/

Here are a few small details on its quirks:

* We can specify labels for which machines run the job: `label 'cmake'` These labels correspond the labels chosen when added to the swarm. At the moment, pepper is the only machine running with the cmake label.

* Groovy interprets different sets of quotes differently. Triple double quotes ("""hi""") means to use a multiline string. More on this here: http://groovy-lang.org/syntax.html#all-strings

* Each stage of the pipeline runs a fresh bash with -xe flags. We turn off the e flag to allow building/testing to continue when there are failures.

* Varibles set in the environment stage will propagate through all stages

# Test Data Specifications

Looking into this. Want to see if there is a good plugin to parse a pr's description, or just use some cleverness with git show: https://git-scm.com/docs/git-show

# Adding Machines to the Swarm

Download the swarm jar here: https://plugins.jenkins.io/swarm

This will only allow jobs with the same labels to run on your machine. If you want docker containers to be able to run on your machine, run this command with sudo:

`java -jar path/to/swarm-client.jar -master http://astro-ci.wr.usgs.gov -labels cmake -mode exclusive`

# Multiplatform Builds

It seems the best way to do multi-platform builds is through a configuration matrix on a multi-configuration project.

**Jenkins Docs:** https://wiki.jenkins.io/display/JENKINS/Building+a+matrix+project \
**Some dude's blog post on the subject:** https://www.linkedin.com/pulse/jenkins-multi-configuration-matrix-muhammad-zbeedat

For example, we can create a 1-D configuration matrix where one axis is build-type (lets call it CMake) and the other consists platforms (Linux, CentOS, Fedora etc.) and each build will be broadcasted to the appropriate machines. Machines on an axis can be identified by either by tags or specific nodes. 

The traditional ISIS build strategy of building on progs vi hand-written bash scripts can be replaced by setting up progs as nodes with appropriate names (prog21->Fedora21, prog20->MacOS10.11 etc.) and setup a 2-D configuration matrix with one axis being nightly vs CI builds and a second axis being different progs. One is triggered via crontab and another via webhooks. This way, CI and nightly builds broadcast to all progs automatically and can all be managed in the same project. 

Alternatively, we can setup 2 projects each with 1-D configs matrices.
 
# Leveraging Previous Builds

**Discarding old builds:** https://wiki.jenkins.io/display/JENKINS/Discard+Old+Build+plugin \
**Permanently keeping builds:** https://wiki.jenkins.io/display/JENKINS/Build+Keeper+Plugin \
**Artifacts:** https://wiki.jenkins.io/display/JENKINS/Copy+Artifact+Plugin

We do not want to rebuild nor retest the entirety of ISIS for every single PR. Every combination of node and queue spot has it's own workspace that by default isn't deleted after a build, successful or otherwise. This means that each new build on a particular node has the changes pulled in from the last commit and the CMake command should naturally build only the differences. 

There may exist possible complications by attempting new builds on top broken builds (merge conflicts and such). The Artifact copy plugin may be a better solution to copying over binaries to prevent building from scratch but also have control over what we keep form previous builds. There may exist some incentive to set a up a cleanup schedule for old builds that haven't been touched in a long time, even if the last build was successful. 

# Triggering Builds From Github PRs

**Github PR builder plugin:** https://wiki.jenkins.io/display/JENKINS/GitHub+pull+request+builder+plugin

We have tested Jenkins polling the repository every 2 minutes for new/changed PRs. If a change is detected it triggers the build. This has been removed until docker and conda specifications are complete.

Jenkins cannot report back to Github presumably because it exists internal to USGS. We need to expose this to allow the communication.

Someone with admin rights to the repository should setup a webhook to trigger on change rather than running the expensive cronjob after we can get Github to communicate with Jenkins master. 

Also check out: https://github.com/jenkinsci/ghprb-plugin#job-dsl-support