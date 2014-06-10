import socket
import urllib

f = open("/home/sudipto/code/l3cube/weblog/ip")
s = f.read().split()

for i in range(0, len(s)):
	if i%2:
		ip = s[i]
		url = "http://api.hostip.info/get_html.php?ip=" + ip
		response = urllib.urlopen(url)
		rs = response.read().split()
		printnow = 0
		for j in rs:
			if j == "City:":
				printnow = 0
			if printnow:
				print j,
			if j == "Country:":
				printnow = 1
			if j == "IP:":
				printnow = 1
		print '\n'
