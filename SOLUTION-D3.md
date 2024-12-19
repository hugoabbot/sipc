# Deliverable 3 ##

### Design Approaches ###

Jackson: Most of what I implemented was straightforward in retrospect. However, at one time or another I thought that the type
constraints for the iterator-style for loop and array reference operator required use of the \alpha type in that we needed to
separately test that the expressions in the array positions were arrays of something, i.e. \alpha. Ultimately, I realized
that a fewer number of only slightly more complex type constraints could be employed in these cases to eliminate using \alpha. In the case
of iterator-style for loops, it is sufficient to verify that the type of the expression in the array position is an array containing
the type of the expression in the variable position. For array reference, a similar approach is taken by verifying that the
type of the expression in the array position is an array containing the type of the overall reference expression.

Hugo: Boolean and array types were by the far the most time consuming part for my work. I modeled boolean off of the previous int type
implementation, which was pretty easy. Array was designed off the construction of reference with some slight modification. Increment and 
decrement statements, unary not, and negation followed the similar pattern as pointer dereference. Lastly, the tenrary operator was a little 
tricky to think about easy to implement. Various changes were made to previous type constraints in `TypeConstraintVisitor.cpp`, including the 
if statement and while loop. These were changed to be boolean type after its creation. I would go to say that this deliverable was a little
bit harder than the previous one dealing with the AST builder just because there so many different moving parts (i.e. concrete creation, 
substituter, checking assignable). Finally, some small changes were made to our AST treetype classes for negation and array length in order
to get correctly formated information from the created type.

### Testing Approaches ###

A total of 34 additional uint tests were written to verify this deliverable. Full coverage of our changes have been made in addition to edge/special
cases, such as the type of an empty array. Much of the testing was merged into the previous testing files instead of making separate SIP files
like in our previous deliverable's tests. Only one problem arose during testing: `cmpassignment.tip` failing after the implementation of the boolean
type and changing of the while loop type constraints. For this, we changed the output of the main function to not be a boolean value, which was 
confirmed later by a Piazza post (`main()` can only return int values).

### Distribution of Work ###

Generally speaking, we divided work for this deliverable by feature such that each of us implemented and tested each of our assigned features.
Hugo handled boolean expressions, array constructors, increment and decrement operators, ternary operator, unary not, and negation. Jackson tackled
the for loops, array length and access expressions, and constraint testing for empty arrays.
