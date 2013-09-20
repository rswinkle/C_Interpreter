C_Interpreter
=============


This started as C port of an old college assignment
in C++.  You can see that in the first commit though I
might have made some minor changes/improvements during
port.

The tests have been updated to match the current
state but not to extensively test the new functionality.
I'll get to that.

Now my goal is to make something approaching scriptable
C.  I'll consider it done when it can run itself ...
so it'll probably never be completely done.

I'm using the 5th edition of C A Reference Manual for
all the nitty gritty details and the very convenient
complete C grammar/syntax in the appendix.

The grammar below is the current status more or less.
It'll be interesting to see how it grows and which
parts converge with full syntax first. 

I've also added a BNF spec for C I found online just
to have something for reference in the repository.


Current Grammar (work in progress)
==================================
```

translation_unit          -> top_level_declaration
                             translation_unit top_level_declaration

declaration_list          -> declaration
                          -> function_definition

function_definition       -> declaration_specifier function_declarator compound_statement

function_declarator       -> identifier '(' parameter_list ')'

parameter_list            -> parameter_declaration
                             parameter_list ',' paramater_declaration

parameter_declaration     -> declaration_specifier identifier

declaration               -> declaration_specifier initialized_declarator_list ';'

initialized_declarator_list     -> initialized_declarator
                                   initialized_declarator_list ',' initialized_declarator

initialized_declarator    -> identifier
                          -> identifier '=' assign_expr

declaration_specifier     -> int

compound_statement        -> '{' decl_or_stmt_list '}'

decl_or_stmt_list         -> decl_or_stmt
                             decl_or_stmt_list decl_or_stmt

decl_or_stmt              -> declaration
                             statement

statement                 -> expr_stmt
                             while_stmt
                             if_stmt
                             print_stmt
                             compound_statement
                             return_stmt
                             goto_stmt
                             labeled_stmt
                             break_or_continue_stmt

break_or_continue_stmt    -> break ';'
                             continue ';'

labeled_stmt              -> identifier ':' statement

while_stmt                -> while '(' expr ')' statement

for_stmt                  -> for for_expressions statement
for_expressions           -> '(' initial_clause<opt> ';' expr<opt> ';' expr<opt> ')'
initial_clause            -> expr
                          -> declaration

if_stmt                   -> if '(' expr ')' statement
						  -> if '(' expr ')' statement else statement

print_stmt                -> print identifier ';'

goto_stmt                 -> goto identifier ';'

expr_stmt                 -> expr ';'

expr                      -> comma_expr

comma_expr                -> assign_expr
                             assign_expr ',' assign_expr

assign_expr               -> cond_expr
                          -> identifier assign_op assign_expr

assign_op                 -> one of '=' '+=' '-=' '*=' '/=' '%='

cond_expr                 -> logical_or_expr  //could add ternary here

logical_or_expr           -> logical_and_expr
                             logical_or_expr '||' logical_and_expr

logical_and_expr          -> equality_expr
                             logical_and_expr '&&' equality_expr

equality_expr             -> relational_expr
                             equality_expr '==' relational_expr
                             equality_expr '!=' relational_expr

relational_expr           -> add_expr
                             relational_expr relational_op add_expr

relational_op             -> one of '<' '>' '<=' '>='

add_expr                  -> mult_expr
                             add_expr add_op mult_expr

add_op                    -> one of '+' '-'

mult_expr                 -> unary_expr
                             mult_expr mult_op unary_expr

mult_op                   -> one of '*' '/' '%'

unary_expr                -> postfix_expr
                             logical_negation_expr
                             preincrement_expr
                             predecrement_expr

preincrement_expr         -> '++' unary_expr
predecrement_expr         -> '--' unary_expr

logical_negation_expr     -> '!' unary_expr


postfix_expr              -> function_call
                             primary_expr
                             postincrement_expr
                             postdecrement_expr

postincrement_expr        -> postfix_expr '++'
postdecrement_expr        -> postfix_expr '--'

function_call             -> identifier '(' expression_list ')'

expression_list           -> assign_expr
                             expression_list ',' assign_expr

primary_expr              -> identifier
                             constant
                             '(' expr ')'
```
