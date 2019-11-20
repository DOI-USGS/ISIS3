// vim: ft=groovy

def myFunc(scm) {
    println("IN MY FUNCTION")
    sh "echo ${scm}"
    checkout scm
}

return this
