/* A Bison parser, made by GNU Bison 3.0.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2013 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

#ifndef YY_SHELLYY_SHELLPARSER_HPP_INCLUDED
# define YY_SHELLYY_SHELLPARSER_HPP_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int shellyydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    TL_IDENT = 258,
    TL_REAL = 259,
    TK_IMPORT = 260,
    TK_CREATE = 261,
    TK_CONNECT = 262,
    TO_SEMI = 263,
    TO_COMMA = 264,
    TS_OBR = 265,
    TS_CBR = 266
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE YYSTYPE;
union YYSTYPE
{
#line 13 "ShellParser.y" /* yacc.c:1915  */

  ps::shell::Command *command;
  ps::shell::Commands *commands;
  ps::shell::Import *import;
  ps::shell::Create *create;
  ps::shell::Connect *connect;
  ps::shell::CreateFormat *c_fmt;
  ps::shell::CreateArgs *c_args;
  ps::shell::CreateTarget *c_tgt;
  ps::shell::CreateTargets *c_tgts;
  std::vector<std::string> *strings;
  ps::shell::Connection *connection;
  ps::shell::Connections *connections;
  std::string *string;
  int token;

#line 83 "ShellParser.hpp" /* yacc.c:1915  */
};
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE YYLTYPE;
struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif


extern YYSTYPE shellyylval;
extern YYLTYPE shellyylloc;
int shellyyparse (void);

#endif /* !YY_SHELLYY_SHELLPARSER_HPP_INCLUDED  */
