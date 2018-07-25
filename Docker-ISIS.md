WORK IN PROGRESS (Eventually Going to be a jenkins/docker/conda guide)

Installation Guide: https://docs.docker.com/install/linux/docker-ce/fedora/

(For Fedora25, download/install using the repository)

**The Docker Daemon**

On Fedora 25, you need to manually start/stop the docker daemon: 

`sudo systemctl start docker`

`sudo systemctl stop docker`

**Pulling Containers**

Fedora 25 image used in combination with our conda environment:
`sudo docker pull chrisryancombs/docker_isis`

**Running Containers**

Docker run reference: https://docs.docker.com/engine/reference/run/

You will need a directory named 'workspace' with your clone of ISIS3_cmake in it.

`sudo docker run -i -t -v /usgs/cpkgs/isis3/data:/usgs/cpkgs/isis3/data -v /usgs/cpkgs/isis3/testData:/usgs/cpkgs/isis3/testData -v /path/to/workspace/:/workspace:rw,z chrisryancombs/docker_isis /bin/bash`

**Maintenance**

Clean up your docker images so that /var doesn't hate you: 

`docker system prune -a -f`