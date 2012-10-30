
def main():
    startApplication("qnet")
    waitFor("object.exists(':File.E&xit_QAction')", 20000)
    test.compare(findObject(":File.E&xit_QAction").text, "E&xit")
    clickButton(waitForObject(":qnet.Exit_QToolButton"))


