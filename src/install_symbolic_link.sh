#!/bin/bash

if test -d $HOME/bin; then
	if test -L $HOME/bin/jitterhistv16; then
		# echo "File jitterhistv16 exists...deleting file to replace it with new version."
		rm $HOME/bin/jitterhistv16
	fi
	ln -s $PWD/jitterhistv16 $HOME/bin/jitterhistv16
	# echo "File jitterhistv16 created..."
	# ls -l $HOME/bin/jitterhistv16
fi
