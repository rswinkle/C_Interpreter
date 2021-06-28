C_Interpreter
=============

[![Build Status](https://travis-ci.org/rswinkle/C_Interpreter.svg?branch=master)](https://travis-ci.org/rswinkle/C_Interpreter)
[![Coverity Scan Build Status](https://scan.coverity.com/projects/3624/badge.svg)](https://scan.coverity.com/projects/3624)

[http://www.robertwinkler.com/projects/c_interpreter.html](http://www.robertwinkler.com/projects/c_interpreter.html)

[![Run on Repl.it](https://repl.it/badge/github/rswinkle/C_Interpreter)](https://repl.it/github/rswinkle/C_Interpreter)


This started as C port of an old college assignment
in C++.  You can see that in the first commit though I
might have made some minor changes/improvements during
port.

Now my goal is to make something approaching scriptable
C.  I'll consider it done when it can run itself ...
so it'll probably never be completely done.

Download
========
Get the source from [Github](https://github.com/rswinkle/C_Interpreter).


Alternatives
============
If you're looking for something more professional there's

* [Cling](http://root.cern.ch/drupal/content/cling)
* [Ch](http://www.softintegration.com/)

* [TCC](http://en.wikipedia.org/wiki/Tiny_C_Compiler), a compiler that's so
fast and self-contained they have a command line switch -run to immediately
run it after compiling so you can use C as a JIT scripting language,


Tests
=====
I've kept the old tests around (updating them so they
keep working) but I've been adding new tests for
new features.  They're not comprehensive and they
don't test every edge case but they're growing.


Misc
====
I'm using the 5th edition of C: A Reference Manual for
all the nitty gritty details and the very convenient
complete C grammar/syntax in the appendix.

I've also added a BNF spec for C I found online just
to have something for reference in the repository.

Current Grammar (work in progress)
==================================
### Preprocessor

	#include "relative path based on current directory"
	#define name
	#define name sequence_of_tokens
	#define name ( identifier-list ) sequence_of_tokens
	#undef name
	# (null directive)

It also of course supports actually using the macros.

### C language

	translation_unit                -> top_level_declaration
	                                   translation_unit top_level_declaration

	top_level_declaration           -> declaration
	                                   function_definition

	function_definition             -> declaration_specifier function_declarator compound_statement

	function_declarator             -> identifier '(' parameter_list ')'

	parameter_list                  -> parameter_declaration
	                                   parameter_list ',' paramater_declaration

	parameter_declaration           -> declaration_specifier identifier

	declaration                     -> declaration_specifier initialized_declarator_list ';'

	initialized_declarator_list     -> initialized_declarator
	                                   initialized_declarator_list ',' initialized_declarator

	initialized_declarator          -> identifier
	                                   identifier '=' assign_expr

	declaration_specifier           -> char, signed char
	                                   short, short int, signed short, signed short int
	                                   int, signed int, signed
	                                   long, long int, signed long, signed long int

	                                   unsigned char
	                                   unsigned short, unsigned short int
	                                   unsigned, unsigned int
	                                   unsigned long, unsigned long int

	                                   float
	                                   double

	compound_statement              -> '{' decl_or_stmt_list '}'

	decl_or_stmt_list               -> decl_or_stmt
	                                   decl_or_stmt_list decl_or_stmt

	decl_or_stmt                    -> declaration
	                                   statement

	statement                       -> expr_stmt
	                                   while_stmt
	                                   for_stmt
	                                   do_stmt
	                                   if_stmt
	                                   print_stmt
	                                   compound_statement
	                                   return_stmt
	                                   goto_stmt
	                                   labeled_stmt
	                                   break_or_continue_stmt
	                                   switch_stmt
	                                   case_or_default_stmt
	                                   null_stmt

	null_stmt                       -> ';'

	switch_stmt                     -> switch '(' expr ')' statement

	case_or_default_stmt            -> case constant_expr ':'
	                                   default ':'

	constant_expr                   -> cond_expr  //nothing but integer literals allowed
	                                              //see tests/switch.c

	break_or_continue_stmt          -> break ';'
	                                   continue ';'

	labeled_stmt                    -> identifier ':' statement

	do_stmt                         -> do statement while '(' expr ')' ';'

	while_stmt                      -> while '(' expr ')' statement

	for_stmt                        -> for for_expressions statement

	for_expressions                 -> '(' initial_clause<opt> ';' expr<opt> ';' expr<opt> ')'

	initial_clause                  -> expr
	                                   declaration

	if_stmt                         -> if '(' expr ')' statement
	                                   if '(' expr ')' statement else statement

	print_stmt                      -> print expr ';'

	goto_stmt                       -> goto identifier ';'

	expr_stmt                       -> expr ';'

	expr                            -> comma_expr

	comma_expr                      -> assign_expr
	                                   assign_expr ',' assign_expr

	assign_expr                     -> cond_expr
	                                   identifier assign_op assign_expr

	assign_op                       -> one of '=' '+=' '-=' '*=' '/=' '%='

	cond_expr                       -> logical_or_expr
	                                   logica_or_expr '?' expr ':' cond_expr

	logical_or_expr                 -> logical_and_expr
	                                   logical_or_expr '||' logical_and_expr

	logical_and_expr                -> bit_or_expr
	                                   logical_and_expr '&&' bit_or_expr

	bit_or_expr                     -> bit_xor_expr
	                                   bit_or_expr '|' bit_xor_expr

	bit_xor_expr                    -> bit_and_expr
	                                   bit_xor_expr '^' bit_and_expr

	bit_and_expr                    -> equality_expr
	                                   bit_and_expr '&' equality_expr

	equality_expr                   -> relational_expr
	                                   equality_expr '==' relational_expr
	                                   equality_expr '!=' relational_expr

	relational_expr                 -> shift_expr
	                                   relational_expr relational_op shift_expr

	relational_op                   -> one of '<' '>' '<=' '>='

	shift_expr                      -> add_expr
	                                   shift_expr '<<' add_expr
	                                   shift_expr '>>' add_expr

	add_expr                        -> mult_expr
	                                   add_expr '+' mult_expr
	                                   add_expr '-' mult_expr

	mult_expr                       -> unary_expr
	                                   mult_expr mult_op unary_expr

	mult_op                         -> one of '*' '/' '%'

	unary_expr                      -> postfix_expr
	                                   sizeof_expr
	                                   bit_negation_expr
	                                   logical_negation_expr
	                                   unary_minus_expr
	                                   unary_plus_expr
	                                   preincrement_expr
	                                   predecrement_expr

	preincrement_expr               -> '++' unary_expr
	predecrement_expr               -> '--' unary_expr

	unary_plus_expr                 -> '+' unary_expr
	unary_minus_expr                -> '-' unary_expr

	logical_negation_expr           -> '!' unary_expr

	bit_negation_expr               -> '~' unary_expr

	sizeof_expr                     -> sizeof '(' type-name ')'
	                                -> sizeof unary-expr

	postfix_expr                    -> function_call
	                                   primary_expr
	                                   postincrement_expr
	                                   postdecrement_expr

	postincrement_expr              -> postfix_expr '++'
	postdecrement_expr              -> postfix_expr '--'

	function_call                   -> identifier '(' expression_list ')'

	expression_list                 -> assign_expr
	                                   expression_list ',' assign_expr

	primary_expr                    -> identifier
	                                   constant
	                                   '(' expr ')'


Building and Running
====================
I use [premake](http://premake.github.io/) for generating makefiles and have shell scripts for running
all my tests at once.

	~ $ git clone https://github.com/rswinkle/C_Interpreter.git
	Cloning into 'C_Interpreter'...
	remote: Reusing existing pack: 489, done.
	remote: Total 489 (delta 0), reused 0 (delta 0)
	Receiving objects: 100% (489/489), 168.87 KiB | 301.00 KiB/s, done.
	Resolving deltas: 100% (264/264), done.
	Checking connectivity... done.
	~ $ cd C_Interpreter/
	~/C_Interpreter $ premake4 gmake
	Building configurations...
	Running action 'gmake'...
	Generating build/Makefile...
	Generating build/cinterpreter.make...
	Done.
	~/C_Interpreter $ cd build/
	~/C_Interpreter/build $ make
	==== Building cinterpreter (debug) ====
	...
	Linking cinterpreter
	~/C_Interpreter/build $ ./cinterpreter 
	Usage: ./cinterpreter [-E] script
	~/C_Interpreter/build $ ./cinterpreter ../tests/switch.txt
	[ouhput from switch.txt here]
	~/C_Interpreter/build $ ../runtests.py
	Should be nothing here, only failures produce output
	~/C_Interpreter/build $ ../run_valgrind_tests.py
	Same here.  Note this may take a minute, valgrind makes things slow
	~/C_Interpreter/build $

