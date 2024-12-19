## Deliverable 1 ##

### Overview ###


Overall, this iteration of the project was not too difficult. One problem we kept running into was whether or not we had enough testing, which will be discussed in a following section. Furthermore, there was some inital difficulty with the placement of new expressions in the ANTLR grammar and the impact on operator precedence. In one instance, the placement of the modulus operator in the list didn't fail the parser testing, but rather a single system test. Lastly, there was some initial difficulty with setting up the compiler but was assisted in office hours.

### Design Approaches ###

From testing, only one major change to our grammar had to be made, specifically with the ternary if statement. The `<assoc=right>` flag had to be made and was confirmed to be the right decision during class discussion. Additionally, separate lexicon symbols were made for increment (`++`) and decrement (`--`) statements. Additional tests showed problems without this implementation strategy, leading to our final design. Finally, two separate array expressions were made, one for fixed length and the other with preset elements. From class discussion, it seemed that this decision isn't mandatory but may make future deliverables easier.

We referred to the C programming language precedence rules (https://en.cppreference.com/w/c/language/operator_precedence) to determine the order in which to place expressions in the .g4 file.

### Testing Approaches ###

A total of 44 unit tests were written to verify the functionality of the SIP Parser. These tests are divided into three groups: equivalency tests, precedence tests, and failing tests.

16 equivalency tests were written to ensure that the new constructs and operators introduced in SIP can be identified by the parser. 
Going down the list of new features from the assignment document, each feature and its obvious permutations is tried.
Originally, there was a larger number of equivalency tests, each with a more specific focus. However, the example set by
the TIP Parser tests was followed, and the tests were consolidated. This approach seems natural because a test which checks for a
positive result passing necessarily means that every component tested is parsable.
to 

9 precedence tests were written to ensure that the SIP Parser adheres to the standard C precedence rules. These tests
verified the correct binding strength of the new boolean operators relative to each other as well as the negation and
modulo operators in relation to other arithmetic operators. Additionally, a test for the proper parsing of nested
ternary operator statements was included.

19 tests in which the parse was intended to fail in order to verify the SIP Parsers selectivity. Again going down the
list of new features from the assignment, operators and structures were written in inappropriate ways. A primary focus
was excluding required elements of the syntax or adding extraneous elements. These failing tests are more condensed than
the equivalency tests above because each test could not contain more than one error. Inserting more than one error in each test
could have given false positive results in which the Parser ignored one error but actually failed to parse for a different
error.

### Distribution of Work

Jackson handled most of the testing and helped with the grammar. Hugo worked on the grammar and helped where needed with tests. 