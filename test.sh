# compile
gcc -g -pthread project2.c -o project2

echo "reasonable one"
./project2 10000000 500 -1
echo 
echo "big boi"
./project2 70000000 500 -1