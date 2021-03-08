# compile
gcc -g -pthread project2.c -o project2

echo "reasonable one"
./project2 10000000 500 -1

echo 
echo "big boi"
./project2 75000000 500 -1 y 15 4

echo
echo "big boi no multithread"
./project2 75000000 500 123 n

echo 
echo "no seed test"
./project2 5000000 500

# echo 
# echo "big boi diff seed"
# ./project2 75000000 500 123456

# echo
# echo "big boi no multithread diff seed"
# ./project2 75000000 500 123456 n

# echo "larry 4 in a row large boys, diff max threads"
# ./project2 150000000 100 123 y 10 1
# echo
# ./project2 150000000 100 123 y 10 2
# echo
# ./project2 150000000 100 123 y 10 3
# echo
# ./project2 150000000 100 123 y 10 4