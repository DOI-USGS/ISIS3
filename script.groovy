// vim: ft=groovy

def isisDataPath = '/isisData/data'

def isisMgrScripts = '/isisData/data/isis3mgr_scripts'

def isisTestDataPath = "/isisData/testData"

def kakaduIncDir = "/isisData/kakadu"

def isisEnv = [
    "ISIS3DATA=${isisDataPath}",
    "ISIS3TESTDATA=${isisTestDataPath}",
    "ISIS3MGRSCRIPTS=${isisMgrScripts}"
]

def cmakeFlags = [
    "-DJP2KFLAG=ON",
    "-DKAKADU_INCLUDE_DIR=${kakaduIncDir}",
    "-Dpybindings=OFF",
    "-DCMAKE_BUILD_TYPE=RELEASE"
]

def build_ok = true
def errors = []

// Helpers for setting commit status
def getRepoUrl() {
    return sh(script: "git config --get remote.origin.url", returnStdout: true).trim()
}

def getCommitSha() {
    return sh(script: "git rev-parse HEAD", returnStdout: true).trim()
}

def setGitHubBuildStatus(status) {
    def repoUrl = getRepoUrl()
    def commitSha = getCommitSha()

    step([
        $class: 'GitHubCommitStatusSetter',
        reposSource: [$class: "ManuallyEnteredRepositorySource", url: repoUrl],
        commitShaSource: [$class: "ManuallyEnteredShaSource", sha: commitSha],
        errorHandlers: [[$class: 'ShallowAnyErrorHandler']],
        statusResultSource: [
          $class: 'ConditionalStatusResultSource',
          results: [
            [$class: 'BetterThanOrEqualBuildResult', result: 'SUCCESS', state: 'SUCCESS', message: status],
            [$class: 'BetterThanOrEqualBuildResult', result: 'FAILURE', state: 'FAILURE', message: status],
            [$class: 'AnyBuildResult', state: 'FAILURE', message: 'Loophole']
          ]
        ]
    ])
}

return this
