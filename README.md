C_Interpreter
=============


This started as C port of an old college assignment
in C++.  You can see that in the first commit though I
might have made some minor changes/improvements during
port.

The tests have been updated to match the current
state but not to extensively test the new functionality.
I'll get to thatI'll get to that.

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
to have something to reference in the repository.


Current Grammar (work in progress)
==================================
```

program               -> var_decl body
var_decl              -> type_specifier id_list
type_specifier        -> int
id_list               -> identifier ',' id_list
                         identifier

body                  -> '{' stmt_list '}'
stmt_list             -> identifier '=' cond_expr ';'
                         while_stmt
                         if_stmt
                         print_stmt
                         goto_stmt

while_stmt            -> while expr body
if_stmt               -> if expr body
print_stmt            -> print identifier ';'

goto_stmt             -> goto identifier ';' //unfinished

cond_expr             -> logical_or_expr  //could add ternary here

logical_or_expr       -> logical_and_expr
                         logical_and_expr '||' logical_and_expr

logical_and_expr      -> equality_expr  //bitwise ops would go here

equality_expr         -> relational_expr
                         relational_expr '==' relational_expr
                         relational_expr '!=' relational_expr

relational_expr       -> add_expr
                         add_expr '<' add_expr
                         add_expr '>' add_expr
                         add_expr '<=' add_expr
                         add_expr '>=' add_expr

add_expr              -> mult_expr
                         mult_expr '+' mult_expr
                         mult_expr '-' mult_expr

mult_expr             -> primary_expr
                         primary_expr '*' primary_expr
                         primary_expr '/' primary_expr

primary_expr          -> identifier
                         constant
                         '(' cond_expr ')'
```
