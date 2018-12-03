properties([pipelineTriggers([githubPush()])])


def nodes = [:]

nodes["isis-fedora-25"] = {
    node("isis-fedora-25") {
        withEnv(["ISISROOT=" + "${workspace}" + "/build/", "ISIS3TESTDATA=/usgs/cpkgs/isis3/testData/", "ISIS3DATA=/usgs/cpkgs/isis3/data/"]) {
            stage ("Fedora 25") {
                git branch: 'dev', url: 'https://github.com/USGS-Astrogeology/ISIS3.git'
            }
        }
    }
}

nodes["isis-centos-7"] = {
    node("isis-centos-7") {
        withEnv(["ISISROOT=" + "${workspace}" + "/build/", "ISIS3TESTDATA=/usgs/cpkgs/isis3/testData/", "ISIS3DATA=/usgs/cpkgs/isis3/data/"]) {
            stage ("CentOS 7") {
                git branch: 'dev', url: 'https://github.com/USGS-Astrogeology/ISIS3.git'
            }
        }
    }
}

nodes["isis-debian-9"] = {
    node("isis-debian-9") {
        withEnv(["ISISROOT=" + "${workspace}" + "/build/", "ISIS3TESTDATA=/usgs/cpkgs/isis3/testData/", "ISIS3DATA=/usgs/cpkgs/isis3/data/"]) {
            stage ("Debian 9") {
                git branch: 'dev', url: 'https://github.com/USGS-Astrogeology/ISIS3.git'
            }
        }
    }
}

nodes["isis-ubuntu-1804"] = {
    node("isis-ubuntu-1804") {
        withEnv(["ISISROOT=" + "${workspace}" + "/build/", "ISIS3TESTDATA=/usgs/cpkgs/isis3/testData/", "ISIS3DATA=/usgs/cpkgs/isis3/data/"]) {
            stage ("Ubuntu 18.04") {
                git branch: 'dev', url: 'https://github.com/USGS-Astrogeology/ISIS3.git'
            }
        }
    }
}


parallel nodes
