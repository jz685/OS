#!/bin/csh

cd example
pwd
set input_dir = "input"
set output_dir = "output"

## cd $input_dir
## pwd
## cd ..
## cd $output_dir
## pwd
## cd ..

@ i = 1
while ( $#argv > 0 )
	echo "--"
	echo $i
	echo $#argv

	if ("$1" == '*.gif') then
		echo "-gif files"
		foreach file ( `find $input_dir -name "*.gif"`)
			echo $file
			set filename = `basename $file`
			set filedir = `dirname $file`
			echo $filename
			echo $filedir
		end

	else if ("$1" == '*.tiff') then
		echo "-tiff"
	else
		echo "Not Valid Pattern"
	endif

	shift
	@ i = $i + 1
end

echo "Finished"


