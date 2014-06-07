# Verify the birthday paradox
import random

# Performs the experiment for <people> number of people
# the expriment is performed <tries> number of times
def performExpt(people, tries):
	success = 0
	for i in range(0, tries):
		success += runFor(people)
	return float(success)/tries

# Runs the experiment for <people> number of people
# Assigns birthdays randomly to the <people> number of people
# if two birthdays are same, this try succeeds in getting 2 people with the same birthday
# if all birthdays are distinct, the try fails
def runFor(people):
	assigned = [0 for i in range(0, 366)]
	for i in range(0, people):
		birthday = random.randint(1, 365)
		if assigned[birthday] == 1:
			return 1
		assigned[birthday] = 1
	return 0

'''
# To let the user run the experiment for different tuples of (people, tries)
while 1:
	print "Enter number of people (0 to quit) :",
	n = input()
	if n == 0:
		break
	print "Enter number of tries of the expt :",
	tries = input()
	print "Probability of 2 people with the same birthday = %.10f" % performExpt(n, tries)
'''

# Run the expt for 1 to 365 people with 1000 tries each
print "People, Probability"
for i in range(1, 366):
	print i, "%.6f" % performExpt(i, 1000)
