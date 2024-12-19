# Deliverable 4 ##

### Design Approaches ###

Jackson: Because array references (e.g. a[0]) can serve as both r- and l-values, the semantic analysis component needed to
be tweaked. Specifically, the endVisit method for ASTAssignStmt in src/semantic/weeding/CheckAssignable.cpp now checks if the
left-hand side of an assignment statement is an instance of an ASTArrayRefExpr. If so, the function returns without printing
an error. This change causes an error to be thrown while the compiler is being built. However, this error does not prevent the compiler
from being built and passing all tests.

Hugo: The new boolean instructions were pretty easy to implement and involved following the ones that came preloaded with the base tipc compiler. Ternary expressions closely resemble if statements, booleans follow integers, and increment/decrement statements followed that of assign/deref. The two arrays were the trickiest work by far with my first actually unique design for this course being implemented, that being a check for negative numbers in ASTArrayFixed declarations (i.e. [-2 of 2]). This was discussed in class as being “extra credit” and is pretty useful in real-world application. One issue that persists even with this is a zero size declaration (i.e. [0 of 2]), which should just make an empty array. Instead, it presents the following message: 
`[error] Error: Execution error, code: 0`
Although I’d like for it to be a little more descriptive, I thought it was unique to our compiler and a fun side project. 

### Testing Approaches ###

A total of 83 test cases across 14 system test files were written to verify this deliverable. We achieved full coverage of
the changes we made to the codebase. Beyond this, we addressed some edge/special cases, including nested for loops and returning
arrays from functions.

The runtests.sh script was modified to run all the .tip system test files in the test/system/siptests directory. Each file
was compiled by tipc and subsequently executed. If a test case failed to achieve the expected value, an error statement output
the actual value. All of our tests passed because no errors were generated.

Each system test file broadly assesses one new SIP feature or group of features.

A bizarre issue we encountered was that the ordering of functions in test files sometimes affected the outcome. Most notably,
the arrayChange.tip system test file (which assesses writing to arrays through array references) caused a segmentation fault when
changeFunc() came before main(). However, placing changeFunc() after main() allowed the test to pass.

Although we did not write any tests involving poly functions, it appears that the polymorphic tests (e.g. fold.sip) in the
siptests.zip file on Canvas fail in their original state. However, they work when the main() function is placed first.

### Distribution of Work ###

Generally speaking, we divided work for this deliverable by feature such that each of us implemented and tested each of our assigned features.
Hugo handled boolean expressions, array constructors, increment and decrement operators, ternary operator, unary not, and negation. Jackson tackled
the for loops, array length and access expressions, and wrote the tests
