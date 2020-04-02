Native libraries for IPASIR

This project packages native shared libraries of propositional satisfiability
(SAT) solvers so that they can be conveniently used from Apache Maven projects.
The provided SAT solvers implement a common Reentrant Incremental Sat solver API
(reverse: IPASIR). The project is adapted from the IPASIR repository used for
SAT competitions: https://github.com/biotomas/ipasir. Some other SAT solvers
have been added afterwards.
 
LICENSE applies only to the generic parts of this software. Individual SAT
solvers packaged in separate Maven modules are distributed under their own
licenses and/or copyright restrictions provided in the root of the corresponding
Maven modules or in distribution packages, such as zip files.

To compile this software, one has to install Apache Maven version 3 or higher
and standard development tools such as Make and GCC. To build all modules, issue
the following command from this folder: 
 
mvn clean install 