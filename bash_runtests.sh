#!/bin/bash


#new tests
test_array[0]=../tests/backslash.txt
test_array[1]=../tests/break_continue.txt
test_array[2]=../tests/problem6.txt
test_array[3]=../tests/declarations.txt
test_array[4]=../tests/dowhile.txt
test_array[5]=../tests/for_stmt.txt
test_array[6]=../tests/functions.txt
test_array[7]=../tests/goto_label.txt
test_array[8]=../tests/ifelse.txt

test_array[10]=../tests/newtest.txt

test_array[12]=../tests/scope.txt
test_array[13]=../tests/switch.txt
test_array[14]=../tests/ternary.txt

#tests from original project
test_array[15]=../tests/test_assignment_basic1.txt
test_array[16]=../tests/test_assignment_basic2.txt
test_array[17]=../tests/test_assignment_variables1.txt
test_array[18]=../tests/test_assignment_variables2_operators.txt
test_array[19]=../tests/test_assignment_variables_no_init.txt
test_array[20]=../tests/test_control_if1.txt
test_array[21]=../tests/test_control_if2.txt
test_array[22]=../tests/test_control_if3_list2.txt
test_array[23]=../tests/test_control_if3_list.txt
test_array[24]=../tests/test_control_if4_list.txt
test_array[25]=../tests/test_control_ifif1.txt
test_array[26]=../tests/test_control_ifif2.txt
test_array[27]=../tests/test_control_ifif3.txt
test_array[28]=../tests/test_control_ifif4.txt
test_array[29]=../tests/test_control_i_if_f1.txt
test_array[30]=../tests/test_control_i_if_f2.txt
test_array[31]=../tests/test_control_i_if_f3.txt
test_array[32]=../tests/test_control_i_if_f4.txt
test_array[33]=../tests/test_control_i_if_f5.txt
test_array[34]=../tests/test_control_i_if_fif1.txt
test_array[35]=../tests/test_control_i_if_fif2.txt
test_array[36]=../tests/test_control_i_if_f_i_if_f1.txt
test_array[37]=../tests/test_control_i_if_f_i_if_f2.txt
test_array[38]=../tests/test_control_while1.txt
test_array[39]=../tests/test_control_while2.txt
test_array[40]=../tests/test_control_while3_fibonacci.txt
test_array[41]=../tests/test_control_while4.txt
test_array[42]=../tests/test_control_while5.txt
test_array[43]=../tests/test_control_whilewhile6.txt
test_array[44]=../tests/test_control_whilewhile7.txt
test_array[45]=../tests/test_control_whilewhile8.txt
test_array[46]=../tests/test_control_whilewhile9.txt
test_array[47]=../tests/test_control_whilewhile_if1.txt
test_array[48]=../tests/types.txt


preprocessor_test_array[0]=../tests/preprocessor/preprocessor.txt
preprocessor_test_array[1]=../tests/preprocessor/include.c

ret_array_index=0

set_ret_array()
{
	$ret_array[$ret_array_index]=$?
	((ret_array_index++))
	printf "%d %d %d\n" $ret_array_index $ret_array[$ret_array_index] $?
	#if [[ $? ]]
	#then
		#exit 1
	#fi
}


echo -e "\nrunning normal tests\n=================="

for f in ${test_array[*]}
do
	echo "executing $f"
	./cinterpreter $f 1> $f.out 2> $f.err

	# for if I ever both stderr and stdout
	# in the same file
	#./cinterpreter $f &> $f.out
done

for f in ${test_array[*]}
do
	echo "comparing output of $f"
	diff $f.expected $f.out
	if [[ -s $f.expected.err ]]
	then
		diff $f.expected.err $f.err
	fi
done

if [[ -n "$1" ]]
then

	echo -e "\nrunning preprocessor tests\n=========================="
	for f in ${preprocessor_test_array[*]}
	do
		echo "running preprocessor on $f"
		./cinterpreter -E $f > $f.out
	done

	for f in ${preprocessor_test_array[*]}
	do
		echo "comparing preprocessor output of $f"
		diff -b $f.expected $f.out
	done
fi
