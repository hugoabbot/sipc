## Deliverable 2 ##

### Design Approaches ###

Jackson: The range-style for loop has an optional 'by' clause that specifies the step size by which to iterate through the range.
This optional element made the range-style for loop trickier to implement than most of the other features. I considered
incorporating the optional step into the ForRangeStmt class itself by overloading the constructor with a version excluding
the step argument (which would have been automatically filled in with 1). However, to avoid the added complexity of having
multiple constructors, I ultimately decided to implement the optional 'by' clause solely inside the `visitForRangeStmt()` method of the AST Builder, which would have needed to deviate from the boilerplate pattern regardless. If the size of the context's expr vector is 4, then all 4 possible
expressions (including the step expression) are present, and the for is built using the provided step expression. Otherwise, it must be
that only the 3 required expressions are present. In this case, an ASTNumberExpr node containing 1 is created and passed to the ASTForRangeStmt
constructor as the step expression.

Hugo: Much of the work I did was relatively simple and followed the "default" implementation of other generic AST nodes. Within this grouping, the increment/decrement statements, the ternary operator, and unary not operation were relatively fast to make. The boolean type was at first thought to be difficult to extend, but was one of the easier ones due to its simplistic nature. Array construction was more tedious due to the requirement of adding two different implementations, with the default version (i.e. [2, x, 13]) being harder to make. This required me to utilize an array of ASTExpr classes to store the various types and amounts of fields within a potential array. Furthermore, I made both `getChildren()` and `getFields()` methods to accomodate different kinds of element referencing in future changes.

### Testing Approaches ###

A total of 44 additional uint tests were written to verify this deliverable. The tests in SIPASTNodeTests.cpp are primarily
concerned with ensuring the constituent expressions of the newly implemented ASTNodes can be correctly assigned and accessed. There, simple
SIP programs each containing an instance of the node under test are converted into ASTs. Then, the node under test is found in
the generated AST and its components (e.g. elements of an array) are accessed and compared against what would be expected
based on the program text. These tests also exercise the getChildren() method of every new node by verifying that each node
has the expected number of children.

The `print()` methods of the new ASTNodes are checked in one of two places: additional REQUIRE statements in SIPASTNodeTests.cpp or
SIPASTPrinterTest.cpp. In either case, each node under test is, broadly speaking, constructed and subsequently printed
to test whether the print matches expectations. Exercising the `print()` and `getChildren()` methods of all new nodes was required to
obtain full coverage of our added code.

Finally, SIPPrettyPrinterTest.cpp contains tests which demonstrate that the modifications made to the Pretty Printer support
the correct printing of each of the new nodes. Similar to the tests in SIPASTNodeTests.cpp, each node is made under an example program format. This stream of characters is fed into the AST Builder, with the node under test being found and compared to an expected print statement. Much of this code is reformatted from the example tests given from PrettyPrinterTest.cpp. 

Due to this testing, we were able to get a line coverage of 98.8% and a function coverage of 99.4%, with much of these gaps being present in code base sections that were not dealt with or modified. 

### Distribution of Work ###

We divided work for this deliverable by feature such that each of us fully implemented and tested each of our assigned features.
Hugo handled boolean expressions, array constructors, increment and decrement operators, ternary operator, and unary not. Jackson tackled
the for loops, negated expressions, modulo operator, new comparison operators, array length and access expressions, and 'and'
and 'or' operators.