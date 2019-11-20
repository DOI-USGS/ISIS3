// vim: ft=groovy

def myFunc() {
    println("IN MY FUNCTION")
    sh "echo ${scm}"
    checkout scm
}

return this
