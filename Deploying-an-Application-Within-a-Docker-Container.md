Information about what I've discovered deploying spiceit within a docker
container will go here.  This will be different than the Wiki Cole is working
on, but there will probably be some overlap.

`sudo journalctl -xe`  Queries the contents of the systemd journal.  It is useful for debugging problems
related to the docker.service not loading.

Create a file called daemon.json which will be used to config docker.  Place it (using sudo privileges obviously) here:  /etc/docker

Mine looks like this at present:

`{`<br></br>
    `"storage-driver": "overlay2",`<br></br>
    `"graph": "/var/lib/docker",`<br></br>
    `"debug": false,`<br></br>
    `"hosts": ["unix:///var/run/docker.sock","tcp://localhost:6666"]`<br></br>
`}`<br></br>



