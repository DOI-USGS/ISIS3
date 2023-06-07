# Jenkins Pipeline

Most of the build process is specified in a groovy script: https://github.com/DOI-USGS/ISIS3/blob/dev/Jenkinsfile

Refer to this for syntax: https://jenkins.io/doc/book/pipeline/syntax/

Here are a few small details on its quirks:

* We can specify labels for which machines run the job: `label 'cmake'` These labels correspond the labels chosen when a machine added to the swarm. They can also be used to specify a Docker container to build in.

* Groovy interprets different sets of quotes differently. Triple double quotes ("""hi""") means to use a multiline string. More on this here: http://groovy-lang.org/syntax.html#all-strings

* Each stage of the pipeline runs a fresh bash with -xe flags. We turn off the e flag to allow building/testing to continue when there are failures.

* Variables set in the environment stage will propagate through all stages


# Docker Container Specs

To specify a container for use in a Jenkins pipeline, label your agent with one of the following: 

* isis-fedora-25
* isis-ubuntu-1804
* isis-debian-9
* isis-centos-7

The labels above tell the Jenkins server that you want to create your agent using a DOI-USGS Docker image. We currently maintain containers specs for building isis on the systems listed above. Their Dockerfiles can be found here: [link](https://github.com/DOI-USGS/docker_linux_isisdeps/tree/master/jenkins)

These files are automatically pulled and built for DockerHub whenever new changes are made. Their images can be found here: [link](https://hub.docker.com/r/usgsastro/docker_linux_isisdeps/)

These images are built with some system library dependencies of ISIS and an install of miniconda. They have the ISIS test data mounted in the /scratch directory.

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
 

# Triggering Builds From Github PRs

**Github PR builder plugin:** https://wiki.jenkins.io/display/JENKINS/GitHub+pull+request+builder+plugin

We have tested Jenkins polling the repository every 2 minutes for new/changed PRs. If a change is detected it triggers the build. This has been removed until docker and conda specifications are complete.

Jenkins cannot report back to Github presumably because it exists internal to USGS. We need to expose this to allow the communication.

Someone with admin rights to the repository should setup a webhook to trigger on change rather than running the expensive cronjob after we can get Github to communicate with Jenkins master. 

Also check out: https://github.com/jenkinsci/ghprb-plugin#job-dsl-support