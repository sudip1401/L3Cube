#!/bin/sh
if [ -z "$1" ]; then
	echo "Usage: ./extract.sh weblogfile"
	exit 0
fi
file=$1

echo  "Extracting ip.."
awk '{print $1}' $file | sort | uniq -c | sort -fg > ip
echo  "Extracting ident.."
awk '{print $2}' $file | sort | uniq -c | sort -fg > ident
echo  "Extracting request.."
awk 'BEGIN{FS = "\""} {print $2}' $file | sort | uniq -c | sort -fg > request
echo  "Extracting user id.."
awk '{print $3}' $file | sort | uniq -c | sort -fg > uid
echo  "Extracting date & time.."
awk '{print $4,$5}' $file | sort | uniq -c | sort -fg >  datetime
echo  "Extracting status code.."
awk '{print $9}' $file | sort | uniq -c | sort -fg > statuscode
echo  "Extracting response size.."
awk '{print $10}' $file | sort | uniq -c | sort -fg > size
echo  "Extracting user agent.."
awk 'BEGIN{FS = "\""} {print $6}' $file | sort | uniq -c | sort -fg > useragent
echo  "Extracting protocols used.."
awk '{print $4}' request | sort | uniq -c | sort -fg > protocols
echo  "Extracting country from ip.. This may take some time."
python getcountry.py > ipcountry
echo  "Sorting countries.."
sort ipcountry | uniq -c | sort -fr | awk 'BEGIN{FS="[0-9]"}; $2 ~ " " {print $2}' | sort | uniq -c | sort -fgr > countries
echo  "Extraction done."
