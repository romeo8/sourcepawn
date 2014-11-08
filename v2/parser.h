/* vim: set ts=2 sw=2 tw=99 et:
 *
 * Copyright (C) 2012-2014 AlliedModders LLC, David Anderson
 *
 * This file is part of SourcePawn.
 *
 * SourcePawn is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 * 
 * SourcePawn is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * SourcePawn. If not, see http://www.gnu.org/licenses/.
 */
#ifndef _include_sourcepawn_parser_h_
#define _include_sourcepawn_parser_h_

#include <stdio.h>
#include "ast.h"
#include "scanner.h"
#include "scopes.h"

namespace ke {

namespace DeclFlags
{
  static const uint32_t MaybeFunction = 0x01;
  static const uint32_t Variable      = 0x02;
  static const uint32_t Old           = 0x04;
  static const uint32_t Argument      = 0x08;
  static const uint32_t Field         = 0x10;
  static const uint32_t Inline        = 0x20; // Don't require a newline/semicolon.
  static const uint32_t MaybeNamed    = 0x40; // Name is optional.

  static const uint32_t NamedMask = MaybeFunction | Argument | Variable | Field | MaybeNamed;
};

namespace DeclAttrs
{
  static const uint32_t None   = 0x0;
  static const uint32_t Static = 0x1;
  static const uint32_t Public = 0x2;
  static const uint32_t Stock  = 0x4;
};

struct Declaration
{
  NameToken name;
  TypeSpecifier spec;
  bool hasPostDims;
  bool isNew;

  Declaration()
   : hasPostDims(false),
     isNew(false)
  {
  }

  void reset() {
    hasPostDims = false;
    isNew = false;
  }
};

class Parser
{
 public:
  Parser(CompileContext &cc, TranslationUnit *tu);

  ParseTree *parse();

 private:
  bool peek(TokenKind kind);
  bool match(TokenKind kind);
  bool expect(TokenKind kind);
  bool requireTerminator();
  bool requireNewlineOrSemi();
  bool requireNewline();

  enum DeclKind {
    DeclArg,
    DeclFun,
    DeclField
  };

  // Guts of the declaration parsing logic.
  bool reparse_decl(Declaration *decl, uint32_t flags);
  void parse_old_array_dims(Declaration *decl, uint32_t flags);
  bool parse_old_decl(Declaration *decl, uint32_t flags);
  void parse_function_type(TypeSpecifier *spec, uint32_t flags);
  void parse_new_typename(TypeSpecifier *spec, const Token *tok);
  void parse_new_type_expr(TypeSpecifier *spec, uint32_t flags);
  bool parse_new_decl(Declaration *decl, uint32_t flags);

  // Returns false only if a name could not be parsed. Other errors allow
  // parsing to continue.
  bool parse_decl(Declaration *decl, uint32_t flags);

  ExpressionList *dimensions();
  Atom *maybeName();
  Atom *expectName();

  Expression *parseCompoundLiteral();
  Expression *parseStructInitializer(const SourceLocation &pos);

  Expression *primitive();
  Expression *call(Expression *callee);
  Expression *index(Expression *left);
  Expression *prefix();
  Expression *primary();
  Expression *unary();
  Expression *multiplication();
  Expression *addition();
  Expression *shift();
  Expression *bitand_();
  Expression *bitxor();
  Expression *bitor_();
  Expression *relational();
  Expression *equals();
  Expression *and_();
  Expression *or_();
  Expression *ternary();
  Expression *assignment();
  Expression *expression();

  bool matchMethodBind();
  LayoutEntry *parseMethod();
  LayoutEntry *parseAccessor();

  ParameterList *arguments();

  Statement *localVariableDeclaration(TokenKind kind, uint32_t flags = 0);
  Statement *methodmap(TokenKind kind);
  Statement *switch_();
  Statement *enum_();
  Statement *if_();
  Statement *block();
  MethodBody *methodBody();
  Statement *variable(TokenKind tok, Declaration *decl, uint32_t flags);
  Statement *function(TokenKind tok, const Declaration &decl, void *, uint32_t attrs);
  Statement *expressionStatement();
  Statement *while_();
  Statement *do_();
  Statement *for_();
  Statement *statementOrBlock();
  Statement *statement();
  StatementList *statements();
  Statement *return_();
  Statement *typedef_();
  Statement *struct_(TokenKind tok);
  Statement *global(TokenKind tok);

 private:
  CompileContext &cc_;
  PoolAllocator &pool_;
  TranslationUnit *tu_;
  Scanner scanner_;
  bool encounteredReturn_;
  bool allowDeclarations_;
  bool allowSingleLineFunctions_;

  Atom *atom_Float_;
  Atom *atom_String_;
  Atom *atom_underbar_;
  Atom *atom_any_;
};

}

#endif // _include_sourcepawn_parser_h_