#!/bin/bash

if test -d $HOME/bin; then
	if test -L $HOME/bin/jitterhist; then
		# echo "File jitterhist exists...deleting file to replace it with new version."
		rm $HOME/bin/jitterhist
	fi
	ln -s $PWD/jitterhist $HOME/bin/jitterhist
	# echo "File jitterhist created..."
	# ls -l $HOME/bin/jitterhist
fi
