#include "ASTNotExpr.h"
#include "ASTVisitor.h"

void ASTNotExpr::accept(ASTVisitor* visitor){
    if(visitor->visit(this)){
        getBase()->accept(visitor);
    }
    visitor->endVisit(this);
} // LCOV_EXCL_LINE

std::vector<std::shared_ptr<ASTNode>> ASTNotExpr::getChildren() {
    std::vector<std::shared_ptr<ASTNode>> children;
    children.push_back(BASE);
    return children;
} // LCOV_EXCL_LINE

std::ostream &ASTNotExpr::print(std::ostream &out) const {
    out << "not (" << *getBase() << ")";
    return out;
} // LCOV_EXCL_LINE
