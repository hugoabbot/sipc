#include "ASTForIteratorStmt.h"
#include "ASTVisitor.h"

void ASTForIteratorStmt::accept(ASTVisitor *visitor) {
    if (visitor->visit(this)) {
        getElement()->accept(visitor);
        getIterable()->accept(visitor);
        getBody()->accept(visitor);
    }
    visitor->endVisit(this);
} // LCOV_EXCL_LINE

std::ostream &ASTForIteratorStmt::print(std::ostream &out) const {
    out << "for (" << *getElement() << " : " << *getIterable() << ") " << *getBody();
    return out;
} // LCOV_EXCL_LINE

std::vector<std::shared_ptr<ASTNode>> ASTForIteratorStmt::getChildren() {
    std::vector<std::shared_ptr<ASTNode>> children;
    children.push_back(ELEMENT);
    children.push_back(ITERABLE);
    children.push_back(BODY);
    return children;
} // LCOV_EXCL_LINE