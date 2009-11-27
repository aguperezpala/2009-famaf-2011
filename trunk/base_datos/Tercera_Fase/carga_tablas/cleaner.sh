#!/bin/sh
for i in `ls | grep .sql`; 
	do 
		cat $i | sort -u | uniq -u > $i
done;