# Contributing to ISIS3
# Under Construction

#### Table of Contents
[I have a question or a problem!](#i-have-a-question-or-a-problem)

[How can I contribute?](#how-can-i-contribute)
  * [Suggesting enhancements](#redmine-issues)
  * [Working on an existing issue or enhancement](#working-on-an-existing-issue-or-enhancement)
  * [Working on a new issue or enhancement](#working-on-a-new-issue)

[Code Base Contribution Guidelines](#code-base-contribution-guidelines)

[What can I expect from the ISIS3 development team](#What-can-I-expect-from-the-ISIS3-development-team)
  * [Response time](#Response-time)
  * [Feedback](#feedback)

[Development Process Checklists](https://github.com/USGS-Astrogeology/ISIS3/wiki/Developer-Checklists)

[References](#references)

## I have a question or a problem!
If you have a question about or a problem with ISIS3, please see the [Redmine Issues](#redmine-issues) section.

If you have a question or a problem with contributing to our software please contact mshepherd@usgs.gov

## How can I contribute?

### Redmine Issues
Redmine is our ticket tracking tool. If you have a question about, a problem with, or a suggestion for ISIS3, please read our [Guidelines for reporting Redmine issues](https://isis.astrogeology.usgs.gov/fixit/projects/isis/wiki/Guidelines_for_Reporting_Issues). Once you have read that please [submit](https://isis.astrogeology.usgs.gov/fixit/projects/isis/issues/new) a Redmine issue.

### Working on an issue or enhancement
1. Find a issue or enhancement that you want to work on. Check Redmine [issues](https://isis.astrogeology.usgs.gov/fixit/projects/isis/issues) to see if there has been a ticket reported about it. If one does not exist that addresses what you want to work on, make a ticket.
2. Assign the Redmine issue to yourself. If you don't have the permission to do this, please make a note on the issue indicating that you would like to work on it. 
3. Propose a solution on the redmine issue. 
4. Work with an ISIS3 developer, the issue reporter, and any other interested parties to get feedback on your solution. This may be an iterative process.
5. Add an [impact statement](https://isis.astrogeology.usgs.gov/fixit/projects/isis/wiki/Impact_Statement) to the redmine issue. If you don't have the permission to do this, please make a note on the issue with the impact statement.
6. Make the changes on your fork of the ISIS3 main GitHub repo. Follow the [Code Base Contribution Guidelines](#code-base-contribution-guidelines).
7. Follow the Developer's Checklist to ensure your changes are ready for review.
8. Make a pull request. Include the redmine issue number in the title of the pull request. Use the "Fixes #0000" format.
9. Work with your code reviewer, tester, and reporter to improve your changes. Your pull request will not be merged in until all parties approve the changes.
10. Check in your test data. If you are an outside contributer, work with your code reviewer to ensure your tests and test data get checked in.

### Working on a new issue or enhancement
1. Please create a [Redmine issue](#redmine-issues).
2. Indicate in the comments of your issue that you would like to work on the issue or enhancement.
3. Follow the [Working on an existing issue or enhancement](#working-on-an-existing-issue-or-enhancement) steps.
 
## Code Base Contribution Guidelines
The following is a set of guidelines for contributing to ISIS3. 
* Personal style changes will not be accepted.
* Changes to bring code closer to our [Coding Standards and Style Guide](https://isis.astrogeology.usgs.gov/documents/CodingStandards/CodingStandards.html) are encouraged.
* Please be professional, even in comments.
* Variable names must be meaningful.
* All modified code is required to pass our [Coding Standards and Style Guide](https://isis.astrogeology.usgs.gov/documents/CodingStandards/CodingStandards.html).
* You must write or update tests to exercise any modified code.
* You must provide test data for any new or modified tests.
* You must have a Redmine issue assigned to you before submitting a pull request.
* You must complete the Developer Checklist before submitting your pull request.
* When submitting a pull request, you must include the Redmine issue number in the title of the pull request. Use the "Fixes #0000" format (#0000 being the Redmine ticket number).

## What can I expect from the ISIS3 development team?
### Response Time
Our response time will depend on availability and scheduling.
### Feedback
Our feedback will address any discrepancies related to the contribution guidelines, our coding standards and style guide, and our code review checklist.

## References
  * [ISIS3 API Reference](https://isis.astrogeology.usgs.gov/Object/Developer/index.html)
  * [Tutorials](https://isis.astrogeology.usgs.gov/fixit/projects/isis/wiki/ISIS_Online_Workshops)
  * [Application Documentation](https://isis.astrogeology.usgs.gov/Application/index.html)
  * [Coding Standards and Style Guide](https://isis.astrogeology.usgs.gov/documents/CodingStandards/CodingStandards.html)
  * [Documentation](https://isis.astrogeology.usgs.gov/documents/CodingStandards/CodingStandards.html#documentation)
  * [Redmine Issue Lifecycle](https://isis.astrogeology.usgs.gov/fixit/projects/isis/wiki/Life_Cycle_of_an_ISIS_Issue)
  

