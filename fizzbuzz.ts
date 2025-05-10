declare_int $num ; 
assign_int 1 to $num ;
declare_int $rem ;
declare_bool $flag ;
loop 50 times :
    print $num ;
    remainder $num by 3 save to $rem ;
    if $rem is_equal_to 0 print ^fizz ;
    if $rem is_not_equal_to 0 assign_bool false to $flag ;
    remainder $num by 5 save to $rem ;
    if $rem is_equal_to 0 print ^buzz ;
    if $rem is_not_equal_to 0 assign_bool false to $flag ;
    if $flag is_equal_to true print ^fizzbuzz ;
    assign_bool true to $flag ;
    add $num to 1 save to $num ;
|
