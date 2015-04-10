README

Group 1: Olawale Akinnawo, Sherin Mathew, Viivi Raina
------------------------------------------------------------

Included files:

Documentation:
	README.txt
	Report.docx
	Mathew, S. K. Ind Jour.docx
	Raina Ind Jour.docx
	
Source code:
	client.c
	server.c
	data.txt
	atm.txt
	
------------------------------------------------------------
Set up Server:

	1. Put server.c in any directory in the server machine
	2. Put data.txt and atm.txt in the same directory as server.c
	3. Change variable "SERVER_PORT" in server.c to the port you want to use for the server
	4. Compile with "gcc -o server server.c"
	5. Start the server using "./server"
	
------------------------------------------------------------
Set up Client:

	6. Put client.c in any directory in the client machine
	7. Change variable "SERVER_IP" in client.c the ip address of the server machine
	8. Change variable "SERVER_PORT" in client.c to the same port defined in server.c
	9. Compile with "gcc -o client client.c"
	10. Run client suing "./client"

------------------------------------------------------------
Using client:

	1.	Type in the command number
	2.	Type in the input in the format requested
	3.	Hit ENTER after each input

Commands from the outside ATM menu:
	0. Type "0" and ENTER to Log in
	1. Type "1" and ENTER to Create new account
	8. Type "8" and ENTER to securely close client and server process
Commands from the logged-in ATM user menu:
	2. Type "2" and ENTER to make a deposit
	3. Type "3" and ENTER to make a withdrawal
	4. Type "4" and ENTER to show account balance
	5. Type "5" and ENTER to show last n transactions
	6. Type "6" and ENTER to buy stamps from the ATM
	7. Type "7" and ENTER to log out of the user session