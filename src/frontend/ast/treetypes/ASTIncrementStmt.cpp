#include "ASTIncrementStmt.h"
#include "ASTVisitor.h"

void ASTIncrementStmt::accept(ASTVisitor* visitor){
    if(visitor->visit(this)){
        getBase()->accept(visitor);
    }
    visitor->endVisit(this);
} // LCOV_EXCL_LINE

std::ostream &ASTIncrementStmt::print(std::ostream &out) const {
    out << *getBase() << "++";
    return out;
} // LCOV_EXCL_LINE

std::vector<std::shared_ptr<ASTNode>> ASTIncrementStmt::getChildren(){
    std::vector<std::shared_ptr<ASTNode>> children;
    children.push_back(BASE);
    return children;
} // LCOV_EXCL_LINE