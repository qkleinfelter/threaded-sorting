# compile
gcc -g -pthread project2.c -o project2

echo "reasonable one"
./project2 10000000 500 -1

echo 
echo "big boi"
./project2 75000000 500 123

echo
echo "big boi no multithread"
./project2 75000000 500 123 n