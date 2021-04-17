#Jack Cooper, V00920368, March 20, 2021
def main():
	elements_string = input("Enter elements: ")
	elem = elements_string.split(" ")
	elem = [int(x) for x in elem]
	n = int(input("Enter n: "))
	k = len(elem)
	lamb = int(input("Enter lambda: "))
	x = 0
	checks = list(range(1,n+1))
	checks = [0 for x in checks]
	latex = ""
	temp_str = ""
	count = 0
	for i in elem:
		for j in elem:
			count += 1
			x = (i-j)%n
			if( x != 0):
				checks[x] += 1
			temp_str = str(i) + "-" + str(j) + " = " + str(x)
			print(temp_str)
			latex += temp_str
			if count == 5:
				latex += "\\\\ \n"
			else:
				latex += " & "
		latex += "\\\\ \n"
		count =0
	print("Differences: ")
	for i in range(1, n):
		print( "Value: " + str(i) + " appears " + str(checks[i]))
		
	for i in range(1, n):
		if (checks[i] != lamb):
			print("Not a difference set: " + str(i) + " appears " + str(checks[i]) + " times.")
			return
	print("Congrats it is a difference set!")
	print(latex)


if __name__ == "__main__":
	main()