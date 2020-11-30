#! /bin/sh

make clean
make
./writetests
testFiles=$(ls *.um $2)
cd ..
make um
for testFile in $testFiles ; do
    echo
    echo $testFile
    testName=$(echo $testFile | sed -E 's/(.*).um/\1/')
    output="um-lab/${testName}.1"
    input="um-lab/${testName}.0"
    if [ -f $output ] ; then
        if [ -f $input ] ; then
            valgrind ./um um-lab/$testFile < ${input} > um-lab/${testName}.txt
        else
            valgrind ./um um-lab/$testFile > um-lab/${testName}.txt
        fi
            diff um-lab/${testName}.1 um-lab/${testName}.txt
            diff -s um-lab/${testName}.1 um-lab/${testName}.txt
    else
        if [ -f $input ] ; then
            valgrind ./um um-lab/$testFile < ${input}
        else
            valgrind ./um um-lab/$testFile
        fi
    fi
done
