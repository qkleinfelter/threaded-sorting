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

echo
echo "y multithread"
./project2 5000000 500 -1 y

echo
echo "2 pieces - default 4 threads, should error"
./project2 5000000 500 -1 y 2

echo
echo "2 pieces / 4 threads should error"
./project2 5000000 500 -1 y 2 4

echo
echo "2 pieces / 2 threads should work"
./project2 5000000 500 -1 y 2 2
