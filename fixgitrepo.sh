cd /home/osc
echo "deleting old backups"
rm -rf project2-backup
echo "creating new backup"
cp -R project2 project2-backup
echo "recloning from git"
git clone git@github.com:qkleinfelter/threaded-sorting project2-newclone
sleep 1
echo "removing old .git"
rm -rf project2/.git
sleep 1
echo "moving cloned git to project2"
mv project2-newclone/.git project2
sleep 1
echo "deleting clone"
rm -rf project2-newclone