# Code Review Checklist
The purpose of this checklist is to prompt discussion between the reviewer and submitter. Any questions or concerns that come from this checklist should be discussed during the code review.

* Did they complete their developer checklist? Double check their checklist. 
* All code changes must have a history comment.
* Are all of their changes tested? Do they have 100% line/scope/function coverage for their changes? 
* Does the redmine ticket have an impact statement that is meaningful from a user’s point of view?
* Does the code work? Does it perform its intended function, the logic is correct etc?
* Does the code reinvent the wheel?
* Is the code a duplication of functionality already contained within ISIS?
* Do the changes make sense (in terms of the big picture/ticket/etc.)?
* Does any of the code stand out as unusual?
* Does the code handle potential exceptions, return values, invalid inputs...?
* Does all this code need to exist? (e.g. extraneous mutators etc.)
* If you had to extend the code, could you do it relatively easily?
* Are they adding [tech debt](https://www.agileweboperations.com/technical-debt)?
* Are there new dependencies between classes/components/modules? Are they necessary?
* Is there any unnecessary code duplication?
* Is the code easily understandable (i.e. [squint test](https://robertheaton.com/2014/06/20/code-review-without-your-eyes/))?
* Scalability? Is the code performance acceptable with very large data sets? Lots of small images? A few very large images?
* Is the code appropriately [modular](https://www.tutorialspoint.com/software_engineering/software_design_basics.htm)?
* Does the code do more than needed to solve the problem at hand? 
* Is the code needlessly complex?
* Is there low-hanging fruit that is easily refactorable?
* Is this code in the right place? (are the correct classes/apps handling these things, are they at the right level, does the order make sense)
* Do new classes/functions have well-defined, documented responsibilities? Are these responsibilities too broad?
* Are the API documentation and comments useful? Are comments explaining why?
* Are all style choices and changes appropriate?