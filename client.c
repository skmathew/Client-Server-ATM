// Group 1: Olawale Akinnawo, Sherin Mathew, Viivi Raina

/* Single server-client connection (pgs 246-247) */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

/* Server info */
#define SERVER_IP   "129.120.151.94" // cse01.cse.unt.edu
#define SERVER_PORT	65001
#define BUF_SIZE	1024

/* Connection status */
#define NO_CONNECTION	0
#define CONNECTION		1

/* User commands */
#define AUTHENTICATION 0 // Login
#define CREATE_ACCOUNT 1
#define DEPOSIT 2
#define WITHDRAW 3
#define BALANCE 4
#define SHOW_TRANSACTIONS 5
#define STAMPS 6
#define LOGOUT 7
#define CLOSE 8

	/* Send from socket */
	int client_socket;
    struct sockaddr_in	addr_send;
    //int	i;
    char buf_send[BUF_SIZE];
    int	send_len,bytes_sent;
	
	/* Receive from socket */
    struct sockaddr_in	my_addr, recv_addr;
    int	i, addr_size, bytes_received;
    fd_set readfds;
    struct timeval		timeout={0,0};
    int	incoming_len;
    struct sockaddr	remote_addr;
    int	recv_msg_size;
	char buf_receive[BUF_SIZE];
    int	select_ret;

/*---------------------------------------------------------------
Create Account
Command		<101 firstName lastName Pin DL SSN emailAddress>
Response	<103> Account creation failed 
			<104> Account created
			<105> Account already exists
---------------------------------------------------------------*/
int Create_Account() {
    char text[80], input[80];
    int inum=0;
    float fnum=0;
    char * pch;
    
    strcpy(text,"101");
    printf("Please enter first Name: ");
    scanf("%s",input);
    strcat(text," ");
    strcat(text,input);
    printf("Please enter last Name: ");
    scanf("%s",input);
    strcat(text," ");
    strcat(text,input);
    printf("Please enter Pin: ");
    scanf("%s",input);
    strcat(text," ");
    strcat(text,input);
    printf("Please enter DL: ");
    scanf("%s",input);
    strcat(text," ");
    strcat(text,input);
    printf("Please enter SSN with no dashes or spaces: ");
    scanf("%s",input);
    strcat(text," ");
    strcat(text,input);
    printf("Please enter email Address: ");
    scanf("%s",input);
    strcat(text," ");
    strcat(text,input);
    strcat(text," ");

    // Send
    strcpy(buf_send,text);
    send_len=strlen(text);
    bytes_sent=send(client_socket,buf_send,send_len,0);
	
    // Receive
    bytes_received=recv(client_socket,buf_receive,BUF_SIZE,0);
    buf_receive[bytes_received]=0;
	
    pch = strtok (buf_receive," ");
    inum = atoi(pch);
    
    if(inum == 103)
    {
        printf("Account Creation Failed \n");
        return 1;
    }
    else if(inum == 105)
    {
        printf("Account Already Exists \n");
        return 1;
    }
    else if(inum == 104)
    {
        printf("Account Created \n");
        return 0;   
    }
    else if(inum == 908)
    {
        printf("Missing Entry \n");
        return 1;   
    }
    
    else return 0;
}

/*---------------------------------------------------------------
Authentication
Command		<201 firstName PIN>
Response	<203> Authentication failure
			<204> Authentication exceeded 10 failed attempts
			<205> Authentication success
---------------------------------------------------------------*/
int Authentication() {
    char text[80], fname[80], pin[80];
    int inum=0;
    float fnum=0;
    char * pch;
    
    strcpy(text,"201");
    printf("Please enter first name: ");
    scanf("%s",fname);
    strcat(text," ");
    strcat(text,fname);
    printf("Please enter 4 digit PIN: ");
    scanf("%s",pin);
    strcat(text," ");
    strcat(text,pin);
    strcat(text," ");
    
    // Send
    strcpy(buf_send,text);
    send_len=strlen(text);
    bytes_sent=send(client_socket,buf_send,send_len,0);
	
    // Receive
    bytes_received=recv(client_socket,buf_receive,BUF_SIZE,0);
    buf_receive[bytes_received]=0;
	
    pch = strtok (buf_receive," ");
    inum = atoi(pch);
    
    if(inum == 204)
    {
        printf("Authentication Attempts Exceeded\n");
        return -1;
    }
    else if(inum == 203)
    {
        printf("Authentication Failed\n");
        return -1;
    }
    else if(inum == 205)
    {
        printf("Authentication Success\n");
        return 0;
    }
    
    else return -1;
}
/*---------------------------------------------------------------
Deposit
Command		<301 Amount>
			<302> Machine full
Response	<303 Balance> Deposit success
			<304 Balance> Deposit failed
			<305 Balance> Deposit return
---------------------------------------------------------------*/
void Deposit() {
    char text[80], amount[80];
    int inum=0, count=0;
    float fnum=0;
    char * pch;
    
    strcpy(text,"301 ");
    printf("Please enter the amount: ");
    scanf("%s",amount);
    strcat(text," ");
    strcat(text,amount);
    strcat(text," ");
    
    // Send
    strcpy(buf_send,text);
    send_len=strlen(text);
    bytes_sent=send(client_socket,buf_send,send_len,0);

    // Receive
    bytes_received=recv(client_socket,buf_receive,BUF_SIZE,0);
    buf_receive[bytes_received]=0;

    pch = strtok (buf_receive," ");
    inum = atoi(pch);
       
    if(inum == 303)
    {
        while (count<1)
        {
            if (count==0)
            {      
                pch = strtok (NULL, " ");
                fnum = atof(pch);
        
                printf("Deposit Success: Balance is %f: \n",fnum);
            }
            count++;
        }
        count = 0;
    }
    else if(inum == 304)
    {
        while (count<1)
        {
            if (count==0)
            {
                pch = strtok (NULL, " ");
                fnum = atof(pch);
                printf("Deposit Failed: Balance is %f: \n",fnum);
            }
            count++;
        }
        count = 0;
    }
    else if(inum == 302)
    {        
        while (count<1)
        {
            if (count==0)
            {
                pch = strtok (NULL, " ");
                fnum = atof(pch);
                printf("Machine Full : Balance is %f: \n",fnum);
            }
            count++;
        }
        count = 0;
    }
}

/*---------------------------------------------------------------
Withdraw
Command		<401 Amount>
			<402> Empty (ATM doesn't have enough money to make withdrawl)
Response	<403 Balance> Withdraw success
			<404 Balance> Funds low
			<405> Withdraw return
---------------------------------------------------------------*/
void Withdraw() {
    char text[80], amount[80];
    int inum=0, count=0;
    float fnum=0;
    char * pch;
    
    strcpy(text,"401 ");
    printf("Please enter the amount: ");
    scanf("%s",amount);
    strcat(text," ");
    strcat(text,amount);
    strcat(text," ");
    
    // Send
    strcpy(buf_send,text);
    send_len=strlen(text);
    bytes_sent=send(client_socket,buf_send,send_len,0);
	
    // Receive
    bytes_received=recv(client_socket,buf_receive,BUF_SIZE,0);
    buf_receive[bytes_received]=0;
    
    pch = strtok (buf_receive," ");
    inum = atoi(pch);
    
    if(inum == 402)
    {
        printf("ATM Empty");
    }
    else if(inum == 405)
    {
        printf("Withdraw Failed");
    }
    else if(inum == 404)
    {
        while (count<1)
        {
            if (count==0)
            {
                pch = strtok (NULL, " ");
                fnum = atof(pch);
                printf("Funds Low : Balance is %f: \n",fnum);
            }
            count++;
        }
        count = 0;
    }    
    else if(inum == 403)
    {
        while (count<1)
        {
            if (count==0)
            {
                pch = strtok (NULL, " ");
                fnum = atof(pch);
                printf("Withdraw Success : Balance is %f: \n",fnum);
            }
            count++;
        }
        count = 0;
    }
    //printf("Test 5\n");
}

/*---------------------------------------------------------------
Balance
Command		<501>
Response	<503 Balance> Return user balance
---------------------------------------------------------------*/
void Balance() {
    char text[80];
    int inum=0, count=0;
    float fnum=0;
    char * pch;
    
    strcpy(text,"501");
    strcat(text," ");
    
    // Send
    strcpy(buf_send,text);
    send_len=strlen(text);
    bytes_sent=send(client_socket,buf_send,send_len,0);
	
    // Receive
    bytes_received=recv(client_socket,buf_receive,BUF_SIZE,0);
    buf_receive[bytes_received]=0;
    
    pch = strtok (buf_receive," ");
    inum = atoi(pch);
    
    if(inum == 503)
    {
        while (count<1)
        {
            if (count==0)
            {
                pch = strtok (NULL, " ");
                fnum = atof(pch);
                printf("Balance is %f: \n",fnum);
            }
            count++;
        }
        count = 0;
    }
    else
    {
        printf("Balance Failed\n");
    }
}

/*---------------------------------------------------------------
Show Transactions
Command		<601 numOfTransactions> 
Response	<603 TransactionCount TransactionMessage> Return transactions
---------------------------------------------------------------*/
void Show_Transactions() {
    char n[80],text[81];
    int inum=0, count=0;
    float fnum=0;
    char * pch;
    
    strcpy(text,"601 ");
    printf("How many transactions 1 - 5 : ");
    scanf("%s",n);
    strcat(text," ");
    strcat(text,n);
    strcat(text," ");

    // Send
    strcpy(buf_send,text);
    send_len=strlen(text);
    bytes_sent=send(client_socket,buf_send,send_len,0);
	
    // Receive
    bytes_received=recv(client_socket,buf_receive,BUF_SIZE,0);
    buf_receive[bytes_received]=0;
    
    pch = strtok (buf_receive," ");
    inum = atoi(pch);
    
    if(inum == 603)
    {
        while (count<1)
        {
            if (count==0)
            {
                pch = strtok (NULL, ";");
                //transactions
                printf("%s",pch);
            }
            count++;
        }
        count = 0;
    }
    else
    {
        printf("%d ",inum);
        printf("Transactions Failed\n");
    }
}

/*---------------------------------------------------------------
Stamps
Command		<701 Amount>
			<702> Out of stamps
Response	<703> Terminate transaction
			<704 Balance> Success
			<705> Stamps return
---------------------------------------------------------------*/
void Stamps() {
    char text[80], stamps[80];
    int inum=0, count=0;
    float fnum=0;
    char * pch;
        
    strcpy(text,"701 ");
    printf("Please enter the stamps: ");
    scanf("%s",stamps);
    strcat(text," ");
    strcat(text,stamps);
    strcat(text," ");
    
    // Send
    strcpy(buf_send,text);
    send_len=strlen(text);
    bytes_sent=send(client_socket,buf_send,send_len,0);
	
    // Receive
    bytes_received=recv(client_socket,buf_receive,BUF_SIZE,0);
    buf_receive[bytes_received]=0;
    
    pch = strtok (buf_receive," ");
    inum = atoi(pch);
    
    if(inum == 702)
    {
        printf("Out of Stamps/n");
    }
    else if(inum == 405)
    {
        printf("Stamps Failed");
    }
    else if(inum == 703)
    {
        printf("Funds Low : Balance is %f: \n",fnum);
    }    
    else if(inum == 704)
    {
        while (count<1)
        {
            if (count==0)
            {
                pch = strtok (NULL, " ");
                fnum = atof(pch);
                printf("Stamps Bought: Balance is %f: \n",fnum);
            }
            count++;
        }
        count = 0;
    }
}

/*---------------------------------------------------------------
Logout
Command		<801> 
Response	<803> Logout success
---------------------------------------------------------------*/
int Logout() {
    char amount[80],text[80];
    int inum=0;
    float fnum=0;
    char * pch;
    
    strcpy(text,"801");
    strcat(text," ");
    
    // Send
    strcpy(buf_send,text);
    send_len=strlen(text);
    bytes_sent=send(client_socket,buf_send,send_len,0);
		
    // Receive
    bytes_received=recv(client_socket,buf_receive,BUF_SIZE,0);
    buf_receive[bytes_received]=0;
    
    pch = strtok (buf_receive," ");
    inum = atoi(pch);
    	
    if(inum == 803)
    {
        printf("Logout Success: \n");
        return NO_CONNECTION;
    }
    else
    {
        printf("Logout Failed\n");
        return CONNECTION;
    }
	
    return CONNECTION;
}

/*---------------------------------------------------------------
 Close
 Command		<901>
 Response	<903> Logout success
 ---------------------------------------------------------------*/
int Close() {
    char amount[80],text[80];
    int inum=0;
    float fnum=0;
    char * pch;
    
    strcpy(text,"901");
    strcat(text," ");
    
    // Send
    strcpy(buf_send,text);
    send_len=strlen(text);
    bytes_sent=send(client_socket,buf_send,send_len,0);
    
    printf("logout sent: %s \n",text); 				// TEST
    return NO_CONNECTION; 							// TEST
   
}




/*---------------------------------------------------------------
	MAIN
---------------------------------------------------------------*/
int main(int argc, char *argv[])
{ 
	int connectionStatus = NO_CONNECTION; // set to CONNECTION when there is any connection to server
	int command = -1;
	
	while(1)
	{
		/*-------------------------
		 *	Print user menu
		 *-----------------------*/
		 
			printf("\nATM commands:\n");
			if( connectionStatus == NO_CONNECTION ) {
				/* no connection to server */
				printf("0 Log in \n");
				printf("1 Create new account \n");
                printf("8 Close the Program \n");
			}
			else {
				/* client is connected to server and user is successfully logged in */
				printf("2 Make deposit \n");
				printf("3 Make withdrawal \n");
				printf("4 Show balance \n");
				printf("5 Show transactions \n");
				printf("6 Buy stamps \n");
				printf("7 Log out \n");
			}
			printf("> ");
			
		/*-------------------------
		 *	Read user command
		 *-----------------------*/
		 
			scanf("%i", &command);
		
		/*-------------------------
		 *	Create client socket
		 *	ONLY IF NOT ALREADY CONNECTED TO SERVER!!
		 *-----------------------*/
		if( connectionStatus == NO_CONNECTION )
		{
			client_socket=socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
			if (client_socket < 0) {
				printf("socket() failed\n");
				exit(0);
			}
			
			/* Create socket address structure to connect to */
			
				memset(&addr_send, 0, sizeof (addr_send)); /* zero out structure */
				addr_send.sin_family = AF_INET; /* address family */
				addr_send.sin_addr.s_addr = inet_addr(SERVER_IP);
				addr_send.sin_port = htons((unsigned short)SERVER_PORT);

			/* Connect to server */
			
				i=connect(client_socket, (struct sockaddr *) &addr_send, sizeof (addr_send));
				if (i < 0) {
					printf("connect() failed\n");
				close(client_socket);
					exit(0);
				}
			
			/* Set connection status */
			
				connectionStatus = CONNECTION;
		}
		
		/*-------------------------
		 *	Process user command
		 *-----------------------*/
		if( command == AUTHENTICATION )
        {
            if ( 0 != Authentication() )
                command = LOGOUT; // Go to Logout
                
        }
		else if( command == CREATE_ACCOUNT)
        {
            if ( 0 != Create_Account() )
                command = LOGOUT; // Go to Logout
        }
		else if( command == DEPOSIT )
			Deposit();
		else if( command == WITHDRAW )
			Withdraw();
		else if( command == BALANCE )
			Balance();
		else if( command == SHOW_TRANSACTIONS )
			Show_Transactions();
		else if( command == STAMPS )
			Stamps();
		
		if( command == LOGOUT)
		{
			connectionStatus = Logout();
			close(client_socket);
		}
        if( command == CLOSE)
        {
            connectionStatus = Close();
            close(client_socket);
            return 0;
        }
        
        if (command != CLOSE && command != LOGOUT && command != STAMPS && command != SHOW_TRANSACTIONS &&command != BALANCE && command != DEPOSIT &&command != CREATE_ACCOUNT && command != AUTHENTICATION)
        {
            printf("ERROR! Command %i not supported!\n", command);
            connectionStatus = Logout();
            close(client_socket);
        }
	}
}