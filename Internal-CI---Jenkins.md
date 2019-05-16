> Note: CI is only internally accessible. It is not possible, at this time, to access build information externally.

## Brief
ASC has a Jenkins CI server that runs in a local kubernetes cloud. All of the CI builds are executed inside of docker containers. As of 5.13.19, the CI is running internally, but is not being triggered from Github or pushing build results back to Github.

## Accessing the CI
The CI server run at [`https://jenkins-dev.wr.usgs.gov`](https://jenkins-dev.wr.usgs.gov). It can take a minute or so for the server to respond the first time it is accessed. The server login is the standard login that is used for all internal accounts.

The CI project that stores ISIS3 is called `docker-testing`.

## CI Jobs
ISIS3 CI is broken into X different processing steps for each operating system:

  1. The Jenkins manager launches one or more build containers that are isolated environments.
  1. The kernel makes a shallow checkout (-depth=50) of the Github repository
  1. The [`environment.yml`](https://github.com/USGS-Astrogeology/ISIS3/blob/dev/environment.yml) file that is in the ISIS3 repository is used to create an environment that is used for the build and testing.
  1. ISIS3 is built. (This takes ~20 minutes with `-j4`. The machines have 4 CPUs.)
  1. ISIS3 tests are executed.

## Containers
The build containers are all stored on GitHub in the [jenkins](https://github.com/USGS-Astrogeology/jenkins). The `/workers/isisdeps/<os>` directories contain the [Dockerfile](https://github.com/USGS-Astrogeology/jenkins/blob/master/workers/isisdeps/ubuntu/Dockerfile) associated with each operating system. As of 5.13.19, the Ubuntu containers is functioning as desired.

### Updating containers
When a container should be updated, we can simply push/merge the change to the [jenkins](https://github.com/USGS-Astrogeology/jenkins) repository. A web hook has already been set up to have the pushed changes built on [Docker Hub](https://cloud.docker.com/u/usgsastro/repository/list). The CI will pull the newest version of the container that is available on Docker Hub at the start of each build.

As of 5.16.19: The jenkins repository has been restructured so that each container is located within its own branch. This allows containers to be built individually only when _they_ are modified, rather than all containers being rebuilt when _any_ of them are modified.

### Data directories
The containers all mount the standard path to the ISIS3 test data area. The `ISISROOT` is set to the current build directory inside of the checked out repo.

## Webhook
5.13.19 - The web hook is currently a work in progress.