#include "ASTForRangeStmt.h"
#include "ASTVisitor.h"

void ASTForRangeStmt::accept(ASTVisitor *visitor) {
    if (visitor->visit(this)) {
        getElement()->accept(visitor);
        getLower()->accept(visitor);
        getUpper()->accept(visitor);
        getStep()->accept(visitor);
        getBody()->accept(visitor);
    }
    visitor->endVisit(this);
} // LCOV_EXCL_LINE

std::ostream &ASTForRangeStmt::print(std::ostream &out) const {
    out << "for (" << *getElement() << " : " << *getLower() << " .. " << *getUpper() << " by " << *getStep() << ") " << *getBody();
    return out;
} // LCOV_EXCL_LINE

std::vector<std::shared_ptr<ASTNode>> ASTForRangeStmt::getChildren() {
    std::vector<std::shared_ptr<ASTNode>> children;
    children.push_back(ELEMENT);
    children.push_back(LOWER);
    children.push_back(UPPER);
    children.push_back(STEP);
    children.push_back(BODY);
    return children;
} // LCOV_EXCL_LINE