#!/bin/csh
# CSci4061 Spring 2015 Assignment 1
# Name: Jia Zhang, Zixiao Wang
# Student ID: 5107014, <ID2>

##	echo $#argv 
##	$argv[1] is input_dir
##	$argv[2] is output_dir

if ( $#argv < 2 ) then
	echo "we need "
	goto ""
endif 

@ len = $#argv

## set inputdir = ./input_directory
set inputdir = $1
set outputdir = $2

## check whether directory that we used for input exists. If not , quit.
if ( ! -d $inputdir ) then 
	echo "$inputdir doesn't exist or is not a directory!"
	goto done
endif 

## check whether directory that we used for output exists. If dir does not exist, create it.
if ( ! -d $outputdir ) then 
	echo "Because the directory for output does not exist, system creates it!"
	mkdir $outputdir
endif

# set outputdir = ./xxxtest
#	echo $inputdir
#	echo $outputdir
cd $inputdir		## get into inputdir
#	pwd
#	ls


## check whether all the files in the inputdir directory is readable

shift
shift

while ( $#argv )
	while ( $1 != "*.gif" || $1 != "*.tiff" || $1 != "*.png" )
		shift
	end
	
	foreach file ( find . -name "$1")
		if ( -d $file ) then 
			 if ( ! -d $outputdir/basename $file ) then  
## set variable = `find . -name "*.jpg"`
# @ i = 1
# @ len = $#variable
# echo $i
# echo $len
# echo $variable
# while ( $i <= 5)
#	echo $variable[i]
#	cp $variable[i] $outputdir
#	@ i = $i + 1
## end


touch mypic.html
cat mypic.html
<html>
<head>
<title>Image 01</title>
</head>
<body>
<a href="images/sunset.jpg">
<img src="images/thumbs/sunset_thumb.jpg"/></a>
</body>
</html> 

done:
	exit 0

error:
	exit 1

