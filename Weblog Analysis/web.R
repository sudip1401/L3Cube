data<-read.table("weblog.txt",sep=" ",header=F)
colnames(data)=c("hostIP","user-identifier","userID","date","time","request","status","size","page","user-agent")
data$date=as.Date(data$date,"[%d/%b/%Y")
reqs=as.data.frame(table(data$date))
ggplot(data=reqs, aes(x=as.Date(Var1), y=Freq)) + geom_line() + xlab('Date') + ylab('Requests') + ggtitle('Traffic to Site')
ggsave("traffic.png")
ggplot(data=data, aes(x=format(status))) + geom_bar() + xlab('Status') + ylab('Count') + ggtitle('Status')
ggsave("status.png")
table(data$hostIP)
table(data$page)


/*
	Output of ggplot are files traffic.png and status.png  
*/
