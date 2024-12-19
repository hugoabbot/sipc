#include "ASTBuilder.h"

#include "picosha2.h"

#include "loguru.hpp"
#include <functional>
#include <vector>

using namespace antlrcpp;

ASTBuilder::ASTBuilder(TIPParser *p) : parser{p} {}

std::string ASTBuilder::opString(int op) {
  std::string opStr;
  switch (op) {
  case TIPParser::MUL:
    opStr = "*";
    break;
  case TIPParser::DIV:
    opStr = "/";
    break;
  case TIPParser::ADD:
    opStr = "+";
    break;
  case TIPParser::SUB:
    opStr = "-";
    break;
  case TIPParser::GT:
    opStr = ">";
    break;
  case TIPParser::EQ:
    opStr = "==";
    break;
  case TIPParser::NE:
    opStr = "!=";
    break;
  case TIPParser::MOD:
    opStr = "%";
    break;
  case TIPParser::LT:
    opStr = "<";
    break;
  case TIPParser::LTE:
    opStr = "<=";
    break;
  case TIPParser::GTE:
    opStr = ">=";
    break;
  default:
    throw std::runtime_error(
        "unknown operator :" +
        std::string(ASTBuilder::parser->getVocabulary().getLiteralName(op)));
  }
  return opStr;
}

/*
 * Globals for communicating information up from visited subtrees
 * These are overwritten by every visit call.
 * We use multiple variables here to avoid downcasting of shared smart pointers.
 */
static std::shared_ptr<ASTStmt> visitedStmt = nullptr;
static std::shared_ptr<ASTDeclNode> visitedDeclNode = nullptr;
static std::shared_ptr<ASTDeclStmt> visitedDeclStmt = nullptr;
static std::shared_ptr<ASTExpr> visitedExpr = nullptr;
static std::shared_ptr<ASTFieldExpr> visitedFieldExpr = nullptr;
static std::shared_ptr<ASTFunction> visitedFunction = nullptr;

/**********************************************************************
 * These methods override selected methods in the TIPBaseVisitor.
 *
 * For each rule name in an ANTLR4 grammar a visit method and a
 * "context" class is generated.  The visit methods are defined in
 * TIPBaseVisitor.h and the context classes in TIPParser.h
 *
 * Only the methods for grammar rules that "directly" contain
 * content that must be processed need to be overridden.  For example,
 * for strings that match "program" you need to process the list of
 * functions.
 *
 * If you are new to visitors, it is very important to understand
 * that since you are selectively overriding methods you cannot
 * rely on the call stack to return values (since the values will
 * be lost by the methods you don't override).  Instead you must create
 * your own structure that is local to the visitor to communicate between
 * the calls during the visit.  In the case of this visitor it is the
 * visitedX variables above.
 *
 * Note that the visit methods are required to return a value, but
 * we make no use of that value, so we simply return the empty string ("")
 * since we cannot return a nullptr (leads to a crash in the visitor).
 *
 * Consult context class definitions to understand the type definitions
 * of the fields that contain the program elements captured during the parse.
 * You will access these from the method overrides in your visitor.
 */

std::shared_ptr<ASTProgram> ASTBuilder::build(TIPParser::ProgramContext *ctx) {
  std::vector<std::shared_ptr<ASTFunction>> pFunctions;
  for (auto fn : ctx->function()) {
    visit(fn);
    pFunctions.push_back(visitedFunction);
  }

  auto prog = std::make_shared<ASTProgram>(pFunctions);
  prog->setName(generateSHA256(ctx->getText()));
  return prog;
}

Any ASTBuilder::visitFunction(TIPParser::FunctionContext *ctx) {
  std::shared_ptr<ASTDeclNode> fName;
  std::vector<std::shared_ptr<ASTDeclNode>> fParams;
  std::vector<std::shared_ptr<ASTDeclStmt>> fDecls;
  std::vector<std::shared_ptr<ASTStmt>> fBody;

  bool firstId = true;
  for (auto decl : ctx->nameDeclaration()) {
    visit(decl);
    if (firstId) {
      firstId = !firstId;
      fName = visitedDeclNode;
    } else {
      fParams.push_back(visitedDeclNode);
    }
  }

  bool isPoly = ctx->KPOLY() != nullptr;

  for (auto decl : ctx->declaration()) {
    visit(decl);
    fDecls.push_back(visitedDeclStmt);
  }

  for (auto stmt : ctx->statement()) {
    visit(stmt);
    fBody.push_back(visitedStmt);
  }

  // return statement is always the last statement in a TIP function body
  visit(ctx->returnStmt());
  fBody.push_back(visitedStmt);

  visitedFunction =
      std::make_shared<ASTFunction>(fName, fParams, fDecls, fBody, isPoly);

  LOG_S(1) << "Built AST node for function " << *visitedFunction;

  // Set source location
  visitedFunction->setLocation(ctx->getStart()->getLine(),
                               ctx->getStart()->getCharPositionInLine());
  return "";
}

Any ASTBuilder::visitNegNumber(TIPParser::NegNumberContext *ctx) {
  int val = std::stoi(ctx->NUMBER()->getText());
  val = -val;
  visitedExpr = std::make_shared<ASTNumberExpr>(val);

  LOG_S(1) << "Built AST node " << *visitedExpr;

  // Set source location
  visitedExpr->setLocation(ctx->getStart()->getLine(),
                           ctx->getStart()->getCharPositionInLine());
  return "";
} // LCOV_EXCL_LINE

/*
 * Unfortunately, the context types for binary expressions generated
 * by ANTLR are not organized into a sub-type hierarchy.  If they were
 * we could have a single method to construct binary expression nodes,
 * but as it stands we have some repetitive code to handle the different
 * context types.
 *
 * This might be improved by restructuring the grammar, but then another
 * mechanism for handling operator precedence would be needed.
 */
template <typename T>
void ASTBuilder::visitBinaryExpr(T *ctx, const std::string &op) {
  visit(ctx->expr(0));
  auto lhs = visitedExpr;

  visit(ctx->expr(1));
  auto rhs = visitedExpr;

  visitedExpr = std::make_shared<ASTBinaryExpr>(op, lhs, rhs);

  LOG_S(1) << "Built AST node " << *visitedExpr;

  // Set source location
  visitedExpr->setLocation(ctx->getStart()->getLine(),
                           ctx->getStart()->getCharPositionInLine());
}

Any ASTBuilder::visitAdditiveExpr(TIPParser::AdditiveExprContext *ctx) {
  visitBinaryExpr(ctx, opString(ctx->op->getType()));
  return "";
} // LCOV_EXCL_LINE

Any ASTBuilder::visitRelationalExpr(TIPParser::RelationalExprContext *ctx) {
  visitBinaryExpr(ctx, opString(ctx->op->getType()));
  return "";
} // LCOV_EXCL_LINE

Any ASTBuilder::visitMultiplicativeExpr(
    TIPParser::MultiplicativeExprContext *ctx) {
  visitBinaryExpr(ctx, opString(ctx->op->getType()));
  return "";
} // LCOV_EXCL_LINE

Any ASTBuilder::visitEqualityExpr(TIPParser::EqualityExprContext *ctx) {
  visitBinaryExpr(ctx, opString(ctx->op->getType()));
  return "";
} // LCOV_EXCL_LINE

Any ASTBuilder::visitAndExpr(TIPParser::AndExprContext *ctx) {
  visitBinaryExpr(ctx, "and");
  return "";
} // LCOV_EXCL_LINE

Any ASTBuilder::visitOrExpr(TIPParser::OrExprContext *ctx) {
  visitBinaryExpr(ctx, "or");
  return "";
} // LCOV_EXCL_LINE

Any ASTBuilder::visitParenExpr(TIPParser::ParenExprContext *ctx) {
  visit(ctx->expr());
  // leave visitedExpr from expr unchanged
  return "";
} // LCOV_EXCL_LINE

Any ASTBuilder::visitNumExpr(TIPParser::NumExprContext *ctx) {
  int val = std::stoi(ctx->NUMBER()->getText());
  visitedExpr = std::make_shared<ASTNumberExpr>(val);

  LOG_S(1) << "Built AST node " << *visitedExpr;

  // Set source location
  visitedExpr->setLocation(ctx->getStart()->getLine(),
                           ctx->getStart()->getCharPositionInLine());
  return "";
} // LCOV_EXCL_LINE

Any ASTBuilder::visitVarExpr(TIPParser::VarExprContext *ctx) {
  std::string name = ctx->IDENTIFIER()->getText();
  visitedExpr = std::make_shared<ASTVariableExpr>(name);

  LOG_S(1) << "Built AST node " << *visitedExpr;

  // Set source location
  visitedExpr->setLocation(ctx->getStart()->getLine(),
                           ctx->getStart()->getCharPositionInLine());
  return "";
}

Any ASTBuilder::visitInputExpr(TIPParser::InputExprContext *ctx) {
  visitedExpr = std::make_shared<ASTInputExpr>();

  LOG_S(1) << "Built AST node " << *visitedExpr;

  // Set source location
  visitedExpr->setLocation(ctx->getStart()->getLine(),
                           ctx->getStart()->getCharPositionInLine());
  return "";
} // LCOV_EXCL_LINE

Any ASTBuilder::visitFunAppExpr(TIPParser::FunAppExprContext *ctx) {
  std::shared_ptr<ASTExpr> fExpr = nullptr;
  std::vector<std::shared_ptr<ASTExpr>> fArgs;

  // First expression is the function, the rest are the args
  bool first = true;
  for (auto e : ctx->expr()) {
    visit(e);
    if (first) {
      fExpr = visitedExpr;
      first = false;
    } else {
      fArgs.push_back(visitedExpr);
    }
  }

  visitedExpr = std::make_shared<ASTFunAppExpr>(fExpr, fArgs);

  LOG_S(1) << "Built AST node " << *visitedExpr;

  // Set source location
  visitedExpr->setLocation(ctx->getStart()->getLine(),
                           ctx->getStart()->getCharPositionInLine());
  return "";
}

Any ASTBuilder::visitAllocExpr(TIPParser::AllocExprContext *ctx) {
  visit(ctx->expr());
  visitedExpr = std::make_shared<ASTAllocExpr>(visitedExpr);

  LOG_S(1) << "Built AST node " << *visitedExpr;

  // Set source location
  visitedExpr->setLocation(ctx->getStart()->getLine(),
                           ctx->getStart()->getCharPositionInLine());
  return "";
} // LCOV_EXCL_LINE

Any ASTBuilder::visitRefExpr(TIPParser::RefExprContext *ctx) {
  visit(ctx->expr());
  visitedExpr = std::make_shared<ASTRefExpr>(visitedExpr);

  LOG_S(1) << "Built AST node " << *visitedExpr;

  // Set source location
  visitedExpr->setLocation(ctx->getStart()->getLine(),
                           ctx->getStart()->getCharPositionInLine());
  return "";
} // LCOV_EXCL_LINE

Any ASTBuilder::visitDeRefExpr(TIPParser::DeRefExprContext *ctx) {
  visit(ctx->expr());
  visitedExpr = std::make_shared<ASTDeRefExpr>(visitedExpr);

  LOG_S(1) << "Built AST node " << *visitedExpr;

  // Set source location
  visitedExpr->setLocation(ctx->getStart()->getLine(),
                           ctx->getStart()->getCharPositionInLine());
  return "";
} // LCOV_EXCL_LINE

Any ASTBuilder::visitNullExpr(TIPParser::NullExprContext *ctx) {
  visitedExpr = std::make_shared<ASTNullExpr>();

  LOG_S(1) << "Built AST node " << *visitedExpr;

  // Set source location
  visitedExpr->setLocation(ctx->getStart()->getLine(),
                           ctx->getStart()->getCharPositionInLine());
  return "";
} // LCOV_EXCL_LINE

Any ASTBuilder::visitRecordExpr(TIPParser::RecordExprContext *ctx) {
  std::vector<std::shared_ptr<ASTFieldExpr>> rFields;
  for (auto fn : ctx->fieldExpr()) {
    visit(fn);
    rFields.push_back(visitedFieldExpr);
  }

  visitedExpr = std::make_shared<ASTRecordExpr>(rFields);

  LOG_S(1) << "Built AST node " << *visitedExpr;

  // Set source location
  visitedExpr->setLocation(ctx->getStart()->getLine(),
                           ctx->getStart()->getCharPositionInLine());
  return "";
}

Any ASTBuilder::visitFieldExpr(TIPParser::FieldExprContext *ctx) {
  std::string fName = ctx->IDENTIFIER()->getText();
  visit(ctx->expr());
  visitedFieldExpr = std::make_shared<ASTFieldExpr>(fName, visitedExpr);

  LOG_S(1) << "Built AST node " << *visitedExpr;

  // Set source location
  visitedFieldExpr->setLocation(ctx->getStart()->getLine(),
                                ctx->getStart()->getCharPositionInLine());
  return "";
}

Any ASTBuilder::visitAccessExpr(TIPParser::AccessExprContext *ctx) {
  std::string fName = ctx->IDENTIFIER()->getText();

  visit(ctx->expr());
  auto rExpr = visitedExpr;

  visitedExpr = std::make_shared<ASTAccessExpr>(rExpr, fName);

  LOG_S(1) << "Built AST node " << *visitedExpr;

  // Set source location
  visitedExpr->setLocation(ctx->getStart()->getLine(),
                           ctx->getStart()->getCharPositionInLine());
  return "";
}

Any ASTBuilder::visitDeclaration(TIPParser::DeclarationContext *ctx) {
  std::vector<std::shared_ptr<ASTDeclNode>> dVars;
  for (auto decl : ctx->nameDeclaration()) {
    visit(decl);
    dVars.push_back(visitedDeclNode);
  }
  visitedDeclStmt = std::make_shared<ASTDeclStmt>(dVars);

  LOG_S(1) << "Built AST node " << *visitedDeclStmt;

  // Set source location
  visitedDeclStmt->setLocation(ctx->getStart()->getLine(),
                               ctx->getStart()->getCharPositionInLine());
  return "";
}

Any ASTBuilder::visitNameDeclaration(TIPParser::NameDeclarationContext *ctx) {
  std::string name = ctx->IDENTIFIER()->getText();
  visitedDeclNode = std::make_shared<ASTDeclNode>(name);

  LOG_S(1) << "Built AST node " << *visitedDeclNode;

  // Set source location
  visitedDeclNode->setLocation(ctx->getStart()->getLine(),
                               ctx->getStart()->getCharPositionInLine());
  return "";
}

Any ASTBuilder::visitBlockStmt(TIPParser::BlockStmtContext *ctx) {
  std::vector<std::shared_ptr<ASTStmt>> bStmts;
  for (auto s : ctx->statement()) {
    visit(s);
    bStmts.push_back(visitedStmt);
  }
  visitedStmt = std::make_shared<ASTBlockStmt>(bStmts);

  LOG_S(1) << "Built AST node " << *visitedStmt;

  // Set source location
  visitedStmt->setLocation(ctx->getStart()->getLine(),
                           ctx->getStart()->getCharPositionInLine());
  return "";
}

Any ASTBuilder::visitWhileStmt(TIPParser::WhileStmtContext *ctx) {
  visit(ctx->expr());
  auto cond = visitedExpr;
  visit(ctx->statement());
  auto body = visitedStmt;
  visitedStmt = std::make_shared<ASTWhileStmt>(cond, body);

  LOG_S(1) << "Built AST node " << *visitedStmt;

  // Set source location
  visitedStmt->setLocation(ctx->getStart()->getLine(),
                           ctx->getStart()->getCharPositionInLine());
  return "";
}

Any ASTBuilder::visitIfStmt(TIPParser::IfStmtContext *ctx) {
  visit(ctx->expr());
  auto cond = visitedExpr;
  visit(ctx->statement(0));
  auto thenBody = visitedStmt;

  // else is optional
  std::shared_ptr<ASTStmt> elseBody = nullptr;
  if (ctx->statement().size() == 2) {
    visit(ctx->statement(1));
    elseBody = visitedStmt;
  }

  visitedStmt = std::make_shared<ASTIfStmt>(cond, thenBody, elseBody);

  LOG_S(1) << "Built AST node " << *visitedStmt;

  // Set source location
  visitedStmt->setLocation(ctx->getStart()->getLine(),
                           ctx->getStart()->getCharPositionInLine());
  return "";
}

Any ASTBuilder::visitOutputStmt(TIPParser::OutputStmtContext *ctx) {
  visit(ctx->expr());
  visitedStmt = std::make_shared<ASTOutputStmt>(visitedExpr);

  LOG_S(1) << "Built AST node " << *visitedStmt;

  // Set source location
  visitedStmt->setLocation(ctx->getStart()->getLine(),
                           ctx->getStart()->getCharPositionInLine());
  return "";
} // LCOV_EXCL_LINE

Any ASTBuilder::visitErrorStmt(TIPParser::ErrorStmtContext *ctx) {
  visit(ctx->expr());
  visitedStmt = std::make_shared<ASTErrorStmt>(visitedExpr);

  LOG_S(1) << "Built AST node " << *visitedStmt;

  // Set source location
  visitedStmt->setLocation(ctx->getStart()->getLine(),
                           ctx->getStart()->getCharPositionInLine());
  return "";
} // LCOV_EXCL_LINE

Any ASTBuilder::visitReturnStmt(TIPParser::ReturnStmtContext *ctx) {
  visit(ctx->expr());
  visitedStmt = std::make_shared<ASTReturnStmt>(visitedExpr);

  LOG_S(1) << "Built AST node " << *visitedStmt;

  // Set source location
  visitedStmt->setLocation(ctx->getStart()->getLine(),
                           ctx->getStart()->getCharPositionInLine());
  return "";
} // LCOV_EXCL_LINE

Any ASTBuilder::visitAssignStmt(TIPParser::AssignStmtContext *ctx) {
  visit(ctx->expr(0));
  auto lhs = visitedExpr;
  visit(ctx->expr(1));
  auto rhs = visitedExpr;
  visitedStmt = std::make_shared<ASTAssignStmt>(lhs, rhs);

  LOG_S(1) << "Built AST node " << *visitedStmt;

  // Set source location
  visitedStmt->setLocation(ctx->getStart()->getLine(),
                           ctx->getStart()->getCharPositionInLine());
  return "";
}

std::string ASTBuilder::generateSHA256(std::string tohash) {
  std::vector<unsigned char> hash(picosha2::k_digest_size);
  picosha2::hash256(tohash.begin(), tohash.end(), hash.begin(), hash.end());
  return picosha2::bytes_to_hex_string(hash.begin(), hash.end());
}

// Begin SIP extension
Any ASTBuilder::visitTernaryExpr(TIPParser::TernaryExprContext *ctx) {
  visit(ctx->expr(0));
  auto cond = visitedExpr;
  visit(ctx->expr(1));
  auto thenBody = visitedExpr;
  visit(ctx->expr(2));
  auto elseBody = visitedExpr;

  visitedExpr = std::make_shared<ASTTernaryExpr>(cond, thenBody, elseBody);

  LOG_S(1) << "Built AST node " << *visitedExpr;

  // Set source location
  visitedExpr->setLocation(ctx->getStart()->getLine(),
                           ctx->getStart()->getCharPositionInLine());
  return "";
} // LCOV_EXCL_LINE

Any ASTBuilder::visitIncrementStmt(TIPParser::IncrementStmtContext *ctx) {
  visit(ctx->expr());
  auto base = visitedExpr;
  visitedStmt = std::make_shared<ASTIncrementStmt>(base);

  LOG_S(1) << "Built AST node " << *visitedStmt;

  visitedStmt->setLocation(ctx->getStart()->getLine(),
                           ctx->getStart()->getCharPositionInLine());

  return "";
} // LCOV_EXCL_LINE

Any ASTBuilder::visitDecrementStmt(TIPParser::DecrementStmtContext *ctx) {
  visit(ctx->expr());
  auto base = visitedExpr;
  visitedStmt = std::make_shared<ASTDecrementStmt>(base);

  LOG_S(1) << "Built AST node " << *visitedStmt;

  visitedStmt->setLocation(ctx->getStart()->getLine(),
                           ctx->getStart()->getCharPositionInLine());

  return "";
} // LCOV_EXCL_LINE

Any ASTBuilder::visitBooleanExpr(TIPParser::BooleanExprContext *ctx) {
  bool base = ctx->BOOLEAN()->getText()[0] == 't';
  visitedExpr = std::make_shared<ASTBooleanExpr>(base);

  LOG_S(1) << "Built AST node " << *visitedExpr;

  visitedExpr->setLocation(ctx->getStart()->getLine(),
                           ctx->getStart()->getCharPositionInLine());

  return "";
} // LCOV_EXCL_LINE

Any ASTBuilder::visitArrayDefaultExpr(TIPParser::ArrayDefaultExprContext *ctx) {
  std::vector<std::shared_ptr<ASTExpr>> fields;
  for (auto &field : ctx->expr()) {
    visit(field);
    fields.push_back(visitedExpr);
  }

  visitedExpr = std::make_shared<ASTArrayDefaultExpr>(fields);

  LOG_S(1) << "Built AST node " << *visitedExpr;

  visitedExpr->setLocation(ctx->getStart()->getLine(),
                           ctx->getStart()->getCharPositionInLine());

  return "";
} // LCOV_EXCL_LINE

Any ASTBuilder::visitArrayFixedExpr(TIPParser::ArrayFixedExprContext *ctx) {
  visit(ctx->expr(0));
  auto number = visitedExpr;
  visit(ctx->expr(1));
  auto instance = visitedExpr;
  visitedExpr = std::make_shared<ASTArrayFixedExpr>(number, instance);

  LOG_S(1) << "Built AST node " << *visitedExpr;

  visitedExpr->setLocation(ctx->getStart()->getLine(),
                           ctx->getStart()->getCharPositionInLine());

  return "";
} // LCOV_EXCL_LINE

Any ASTBuilder::visitNotExpr(TIPParser::NotExprContext *ctx) {
  visit(ctx->expr());
  auto base = visitedExpr;
  visitedExpr = std::make_shared<ASTNotExpr>(base);

  LOG_S(1) << "Built AST node " << *visitedExpr;

  visitedExpr->setLocation(ctx->getStart()->getLine(),
                           ctx->getStart()->getCharPositionInLine());

  return "";
} // LCOV_EXCL_LINE

Any ASTBuilder::visitForIteratorStmt(TIPParser::ForIteratorStmtContext *ctx) {
  visit(ctx->expr(0));
  auto element = visitedExpr;
  visit(ctx->expr(1));
  auto iterable = visitedExpr;
  visit(ctx->statement());
  auto body = visitedStmt;
  visitedStmt = std::make_shared<ASTForIteratorStmt>(element, iterable, body);

  LOG_S(1) << "Built AST node " << *visitedStmt;

  // Set source location
  visitedStmt->setLocation(ctx->getStart()->getLine(),
                           ctx->getStart()->getCharPositionInLine());
  return "";
} // LCOV_EXCL_LINE

Any ASTBuilder::visitForRangeStmt(TIPParser::ForRangeStmtContext *ctx) {
  visit(ctx->expr(0));
  auto element = visitedExpr;
  visit(ctx->expr(1));
  auto lower = visitedExpr;
  visit(ctx->expr(2));
  auto upper = visitedExpr;
  std::shared_ptr<ASTExpr> step;
  if (ctx->expr().size() == 4) {
    visit(ctx->expr(3));
    step = visitedExpr;
  } else {
    step = std::make_shared<ASTNumberExpr>(1);
  }
  visit(ctx->statement());
  auto body = visitedStmt;
  visitedStmt = std::make_shared<ASTForRangeStmt>(element, lower, upper, step, body);

  LOG_S(1) << "Built AST node " << *visitedStmt;

  // Set source location
  visitedStmt->setLocation(ctx->getStart()->getLine(),
                           ctx->getStart()->getCharPositionInLine());
  return "";
} // LCOV_EXCL_LINE

Any ASTBuilder::visitNegExpr(TIPParser::NegExprContext *ctx) {
  visit(ctx->expr());
  auto expr = visitedExpr;
  visitedExpr = std::make_shared<ASTNegExpr>(expr);

  LOG_S(1) << "Built AST node " << *visitedStmt;

  // Set source location
  visitedExpr->setLocation(ctx->getStart()->getLine(),
                           ctx->getStart()->getCharPositionInLine());
  return "";
} // LCOV_EXCL_LINE

Any ASTBuilder::visitArrayLenExpr(TIPParser::ArrayLenExprContext *ctx) {
  visit(ctx->expr());
  auto array = visitedExpr;
  visitedExpr = std::make_shared<ASTArrayLenExpr>(array);

  LOG_S(1) << "Built AST node " << *visitedStmt;

  // Set source location
  visitedExpr->setLocation(ctx->getStart()->getLine(),
                           ctx->getStart()->getCharPositionInLine());
  return "";
} // LCOV_EXCL_LINE

Any ASTBuilder::visitArrayRefExpr(TIPParser::ArrayRefExprContext *ctx) {
  visit(ctx->expr(0));
  auto array = visitedExpr;
  visit(ctx->expr(1));
  auto index = visitedExpr;
  visitedExpr = std::make_shared<ASTArrayRefExpr>(array, index);

  LOG_S(1) << "Built AST node " << *visitedStmt;

  // Set source location
  visitedExpr->setLocation(ctx->getStart()->getLine(),
                           ctx->getStart()->getCharPositionInLine());
  return "";
} // LCOV_EXCL_LINE
