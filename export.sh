tarname="morpion-gaetan.renaudeau.tar.gz"
directory="morpion-gaetan.renaudeau"
rm -f $directory
mkdir $directory
cp *.c $directory
cp *.h $directory
cp Makefile $directory
tar cvzf $tarname $directory
rm -f $directory
