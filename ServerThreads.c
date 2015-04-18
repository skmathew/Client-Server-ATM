
	/* Multiple simultaneous clients handled by threads; */
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>	/* system type defintions */
#include <sys/socket.h>	/* network system functions */
#include <netinet/in.h>	/* protocol & struct definitions */

#define BACKLOG	5
#define BUF_SIZE	1024
#define LISTEN_PORT	65020

int threadCount = BACKLOG;

typedef struct Node
{
    int ssn, dl, pin, count;
    int FailedAttempts;
    float balance;
    char fname[80+1], lname[80+1], email[80+1];
    char transcations[5][80+1];
    float tamount[5];
    struct Node* next;
    struct Node* prev;
};
int stamp = 1000;
float fund = 50000.00;
float Dfund = 0.00;



void *client_handler(void *arg);
struct Node* GetNewNode( char *fname, char *lname, int pin, int dl, int ssn, char *email, struct Node* head, char *buf_receive, char *buf_send);
int freelist(struct Node* head, char *buf_receive, char *buf_send);
int WritetoFile(struct Node* head,  char *buf_receive, char *buf_send);
struct Node* ReadfromFile(struct Node* head,  char *buf_receive, char *buf_send);
struct Node* Create_Account(char *fname, char *lname, int pin, int dl, int ssn,char *email, struct Node* head, char *buf_receive, char *buf_send);
int Authentication(char *fname,int pin, struct Node* head,  char *buf_receive, char *buf_send);
int Deposit(float amount, int ssn, struct Node* head, char *buf_receive, char *buf_send);
int Withdraw(float amount, int ssn, struct Node* head, char *buf_receive, char *buf_send);
int Balance(int ssn, struct Node* head, char *buf_receive, char *buf_send);
int ShowTransactions(int ssn, int n, struct Node* head, char *buf_receive, char *buf_send);
int Stamps(float amount, int ssn,struct Node* head, char *buf_receive, char *buf_send);
void Logout(struct Node* head, char *buf_receive, char *buf_send);



int main(int argc, char *argv[]){
    int status, *sock_tmp;
    pthread_t a_thread;
    void *thread_result;

    struct sockaddr_in addr_mine;
    struct sockaddr_in addr_remote;
    int sock_listen;
    int sock_aClient;
    int addr_size;
    int reuseaddr = 1;


    sock_listen = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_listen < 0) {
        perror("socket() failed");
        exit(0);
    }

    memset(&addr_mine, 0, sizeof (addr_mine));
    addr_mine.sin_family = AF_INET;
    addr_mine.sin_addr.s_addr = htonl(INADDR_ANY);
    addr_mine.sin_port = htons((unsigned short)LISTEN_PORT);

    // /* Enable the socket to reuse the address */
    // if (setsockopt(sock_listen, SOL_SOCKET, SO_REUSEADDR, &reuseaddr,
    // 	sizeof(int)) == -1) {
    //     perror("setsockopt");
    // 	close(sock_listen);
    //     exit(1);
    // }

    status = bind(sock_listen, (struct sockaddr *) &addr_mine,
    	sizeof (addr_mine));
    if (status < 0) {
        perror("bind() failed");
        close(sock_listen);
        exit(1);
    }

    status = listen(sock_listen, 5);
    if (status < 0) {
        perror("listen() failed");
        close(sock_listen);
        exit(1);
    }

    addr_size = sizeof(struct sockaddr_in);
    printf("waiting for a client\n");
    while(1) {
    	if (threadCount < 1) {
    		sleep(1);
    	}

    	sock_aClient = accept(sock_listen, (struct sockaddr *) &addr_remote,
            &addr_size);
    	if (sock_aClient == -1){
    		close(sock_listen);
        	exit(1);
    	}

    	printf("Got a connection from %s on port %d\n",
                    inet_ntoa(addr_remote.sin_addr),
                    htons(addr_remote.sin_port));
    	sock_tmp = malloc(1);
    	*sock_tmp = sock_aClient;
    	printf("thread count = %d\n", threadCount);
    	threadCount--;
 		status = pthread_create(&a_thread, NULL, client_handler,
            (void *) sock_tmp);
 		if (status != 0) {
 			perror("Thread creation failed");
 			close(sock_listen);
 			close(sock_aClient);
 			free(sock_tmp);
        	exit(1);
 		}
    }

    return 0;
}

void *client_handler(void *sock_desc) {
	int msg_size;
	char buf[BUF_SIZE];
	int sock = *(int*)sock_desc;
    
    
    int count = 0, inum=0;
    float fnum=0;
    char * pch;
    char firstName[80], lastName[80],email[80];
    int PIN, DL, SSN=-999999999;
    struct Node* head = NULL;
    char buf_receive[BUF_SIZE], text[80],buf_send[BUF_SIZE];
    
    head=ReadfromFile(head,buf_receive,buf_send);

	printf("In client_handler\n");
        if (head == NULL)
        {
        printf("Database reading error\n");
        }
    
	while ((msg_size = recv(sock, buf_receive, BUF_SIZE, 0)) > 0) {

        //printf("Received: %s\n",buf_receive);
        pch = strtok (buf_receive," ");
            inum = atoi(pch);
        
            /* Server-side feedback */
            
            printf("Received inum: %i \n",inum);
            
            if(inum == 101)
            {
                count = 0;
                /* -------------------------
                 *	Create Account
                 * -----------------------*/
                
                while (count<6)
                {
                    if (pch==NULL) {
                        sprintf(buf_send, "908 ");
                        count = 10;
                    }
                    if (count==0) {
                        pch = strtok (NULL, " ");
                        strcpy(firstName,pch);
                    }
                    if (count==1) {
                        pch = strtok (NULL, " ");
                        strcpy(lastName,pch);
                    }
                    if (count==2) {
                        pch = strtok (NULL, " ");
                        PIN = atoi(pch);
                    }
                    if (count==3) {
                        pch = strtok (NULL, " ");
                        DL = atoi(pch);
                    }
                    if (count==4) {
                        pch = strtok (NULL, " ");
                        SSN = atoi(pch);
                    }
                    if (count==5) {
                        pch = strtok (NULL, " ");
                        strcpy(email,pch);
                    }
                    count++;
                }
                if (count!=10)
                    head = Create_Account( firstName, lastName, PIN, DL, SSN, email, head, buf_receive, buf_send);
                count = 0;
            }
            else if(inum == 201)
            {
                count = 0;
                /* -------------------------
                 *	Authentication
                 * -----------------------*/
                
                while (count<2)
                {
                    if (count==0) {
                        pch = strtok (NULL, " ");
                        strcpy(firstName,pch);
                    }
                    if (count==1) {
                        pch = strtok (NULL, " ");
                        PIN = atoi(pch);
                    }
                    count++;
                }
                SSN = Authentication( firstName, PIN, head, buf_receive, buf_send);
                count = 0;
                
                if( SSN == -1 )
                    inum = 801; // Go to Logout
            }
            else if(inum == 301)
            {
                count = 0;
                /* -------------------------
                 *	Deposit
                 * -----------------------*/
                
                while (count<1)
                {
                    if (count==0) {
                        pch = strtok (NULL, " ");
                        fnum = atof(pch);
                    }
                    count++;
                }
                Deposit( fnum, SSN, head, buf_receive, buf_send);
                count = 0;
            }
            else if(inum == 401)
            {
                count = 0;
                /* -------------------------
                 *	Withdraw
                 * -----------------------*/
                
                while (count<1)
                {
                    if (count==0)
                    {
                        pch = strtok (NULL, " ");
                        fnum = atof(pch);
                    }
                    count++;
                }
                Withdraw( fnum, SSN, head, buf_receive, buf_send);
                count = 0;
            }
            else if(inum == 501)
            {
                count = 0;
                /* -------------------------
                 *	Balance
                 * -----------------------*/
                
                Balance(SSN,head, buf_receive, buf_send);
            }
            else if(inum == 601)
            {
                count = 0;
                /* -------------------------
                 *	Show Transactions
                 * -----------------------*/
                
                while (count<1)
                {
                    if (count==0)
                    {
                        pch = strtok (NULL, " ");
                        inum = atoi(pch);
                    }
                    count++;
                }
                ShowTransactions( SSN, inum,head, buf_receive, buf_send);
            }
            else if(inum == 701)
            {
                count = 0;
                /* -------------------------
                 *	Stamps
                 * -----------------------*/
                
                while (count<1)
                {
                    if (count==0)
                    {
                        pch = strtok (NULL, " ");
                        fnum = atof(pch);
                    }
                    count++;
                }
                Stamps( fnum, SSN, head, buf_receive, buf_send);
                count = 0;
            }
            else if( inum == 901 )
            {
                // CLOSE SOCKET
                SSN=-999999999;
                count = 0;
                inum = 801; // Go to logout
            }
            else if( inum != 801 )
            {
                /* -------------------------
                 *	Unknown code <909>
                 * -----------------------*/
                count = 0;
                sprintf(buf_send, "909 ");
            }
            
            /* -------------------------
             *	Logout
             * -----------------------*/
            if( inum == 801 )
            {
                Logout(head,buf_receive,buf_send);
                SSN=-999999999;
                count = 0;
                sprintf(buf_send, "803 ");
                //printf("sending %s",buf_send);
                msg_size =strlen(buf_send);
                msg_size=send(sock,buf_send,msg_size,0);
                break;
            }
            
            //strcpy(buf_send,buf_receive);
            msg_size =strlen(buf_send);
            msg_size=send(sock,buf_send,msg_size,0);


    }

    WritetoFile(head,buf_receive,buf_send); // Write Nodes and ATM data to files
    freelist(head,buf_receive,buf_send);
    
    close(sock);
	free(sock_desc);
	threadCount++;
    // pthread_exit("Thank you for the CPU time");
}









































/*---------------------------------------------------------------
 Creates a new Node and returns pointer to it
 ---------------------------------------------------------------*/
struct Node* GetNewNode( char *fname, char *lname, int pin, int dl, int ssn, char *email, struct Node* head, char *buf_receive, char *buf_send )
{
    // New Node
    struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
    
    // Copy new user info to Node
    newNode->ssn = ssn;
    strcpy( newNode->fname, fname );
    strcpy( newNode->lname, lname );
    newNode->pin = pin;
    newNode->dl = dl;
    strcpy( newNode->email, email );
    
    // Initialize Node variables
    strcpy( newNode->transcations[0], "null" );
    strcpy( newNode->transcations[1], "null" );
    strcpy( newNode->transcations[2], "null" );
    strcpy( newNode->transcations[3], "null" );
    strcpy( newNode->transcations[4], "null" );
    newNode->tamount[0] = 0;
    newNode->tamount[1] = 0;
    newNode->tamount[2] = 0;
    newNode->tamount[3] = 0;
    newNode->tamount[4] = 0;
    
    newNode->balance = 0;
    newNode->count = 0;
    newNode->FailedAttempts = 0;
    
    newNode->prev = NULL;
    newNode->next = NULL;
    
    return newNode;
}

/*---------------------------------------------------------------
 Free up Nodes from memory
 ---------------------------------------------------------------*/
int freelist(struct Node* head, char *buf_receive, char *buf_send)
{
    if (head == NULL)
        return 0; // No nodes
    
    while( head->next != NULL )
    {
        head = head->next; // Go to next Node
        free( head->prev ); // Free previous Node
    }
    free( head ); // Free last Node
    
    return 0;
}

/*---------------------------------------------------------------
 Write Node data to data.txt
 Write ATM data to ATM.txt
 ---------------------------------------------------------------*/
int WritetoFile(struct Node* head, char *buf_receive, char *buf_send)
{
    /* Write Nodes to data.txt */
    
    char c[1000];
    struct Node* temp = head;
    
    FILE *fptr = fopen( "data.txt", "w" );
    if( fptr == NULL )
    {
        printf("Error! Can not open data.txt \n");
        exit(1);
    }
    
    while(temp != NULL)
    {
        fprintf(fptr,"%d ",temp->ssn);
        fprintf(fptr,"%s ",temp->fname);
        fprintf(fptr,"%s ",temp->lname);
        fprintf(fptr,"%d ",temp->pin);
        fprintf(fptr,"%d ",temp->dl);
        fprintf(fptr,"%s ",temp->email);
        fprintf(fptr,"%f ",temp->balance);
        fprintf(fptr,"%d ",temp->count);
        
        fprintf(fptr,"%s ",temp->transcations[0]);
        fprintf(fptr,"%s ",temp->transcations[1]);
        fprintf(fptr,"%s ",temp->transcations[2]);
        fprintf(fptr,"%s ",temp->transcations[3]);
        fprintf(fptr,"%s ",temp->transcations[4]);
        
        fprintf(fptr,"%f ",temp->tamount[0]);
        fprintf(fptr,"%f ",temp->tamount[1]);
        fprintf(fptr,"%f ",temp->tamount[2]);
        fprintf(fptr,"%f ",temp->tamount[3]);
        fprintf(fptr,"%f ",temp->tamount[4]);
        
        fprintf(fptr,"%d ",temp->FailedAttempts);
        
        fprintf(fptr,"\n");
        temp = temp->next;
    }
    
    fclose(fptr);
    
    /* Write ATM data to ATM.txt */
    
    fptr = fopen( "atm.txt", "w" );
    if( fptr == NULL )
    {
        printf("Error! Can not open file ATM.txt! \n");
        exit(1);
    }
    
    fprintf( fptr, "%d ", stamp );
    fprintf( fptr, "%f ", fund );
    fprintf( fptr, "%f ", Dfund );
    fprintf( fptr, "\n" );
    
    fclose( fptr );
    
    return 0;
}

/*---------------------------------------------------------------
 Read Node data from data.txt
 Read ATM data from ATM.txt
 ---------------------------------------------------------------*/
struct Node* ReadfromFile(struct Node* head, char *buf_receive, char *buf_send)
{
    char fname[80+1];
    strcpy(fname,"fname");
    char lname[80+1];
    strcpy(lname,"lname");
    char email[80+1];
    char transcations[80+1];
    strcpy(email,"doe@gmail.com");
    int ssn = 100000000, pin = 1234, dl = 12345678, count=0;
    float balance=0.0;
    
    /* Read Node data from data.txt */
    
    char c[1000];
    FILE *fptr;
    struct Node* newNode;
    
    fptr = fopen( "data.txt", "r" );
    if(fptr==NULL)
    {
        printf("Error!");
        exit(1);
    }
    
    while( fscanf( fptr, "%d ", &ssn )!= EOF )
    {
        // Read user info
        fscanf(fptr, "%s ", fname );
        fscanf(fptr, "%s ", lname );
        fscanf(fptr, "%d ", &pin );
        fscanf(fptr, "%d ", &dl );
        fscanf(fptr, "%s ", email );
        
        // Create new node for user
        newNode = GetNewNode( fname, lname, pin, dl, ssn, email,head,buf_receive,buf_send);
        
        fscanf(fptr, "%f ",&newNode->balance);
        fscanf(fptr, "%d ",&newNode->count);
        
        fscanf(fptr,"%s ",newNode->transcations[0]);
        fscanf(fptr,"%s ",newNode->transcations[1]);
        fscanf(fptr,"%s ",newNode->transcations[2]);
        fscanf(fptr,"%s ",newNode->transcations[3]);
        fscanf(fptr,"%s ",newNode->transcations[4]);
        
        fscanf(fptr,"%f ",&newNode->tamount[0]);
        fscanf(fptr,"%f ",&newNode->tamount[1]);
        fscanf(fptr,"%f ",&newNode->tamount[2]);
        fscanf(fptr,"%f ",&newNode->tamount[3]);
        fscanf(fptr,"%f ",&newNode->tamount[4]);
        
        fscanf(fptr,"%d ",&newNode->FailedAttempts);
        
        if( head == NULL )
            head = newNode; // First node is head of list
        else
        {
            // Set newNode as head
            head->prev = newNode;
            newNode->next = head;
            head = newNode;
        }
    }
    
    fclose(fptr);
    
    /* Read ATM data from ATM.txt */
    
    fptr=fopen( "atm.txt", "r" );
    if(fptr==NULL)
    {
        printf("Error!");
        exit(1);
    }
    
    while(fscanf( fptr, "%d ", &stamp )!= EOF)
    {
        fscanf( fptr, "%f ", &fund );
        fscanf( fptr, "%f ", &Dfund );
    }
    fclose(fptr);
    
    return head;
}

/*---------------------------------------------------------------
 Create Account
 Command		<101 firstName lastName Pin DL SSN emailAddress>
 Response	<103> Account creation failed
 <104> Account created
 <105> Account already exists
 ---------------------------------------------------------------*/
struct Node* Create_Account(char *fname, char *lname, int pin, int dl, int ssn,char *email, struct Node* head, char *buf_receive, char *buf_send)
{
    struct Node* temp = head;
    int result = 99;
    
    while(temp != NULL)
    {
        // Duplicate SSN
        if (temp->ssn == ssn) {
            sprintf( buf_send, "105 " );
            printf( "\t Duplicate SSN: %d \n", ssn ); // Feedback
            return head;
        }
        
        result = strcmp(temp->fname, fname);
        if (result==0)
        {
            if (temp->pin == pin)
            {
                sprintf( buf_send, "105 " );
                printf( "\t Duplicate entry \n"); // Feedback
                return head;
            }
        }
        
        if (temp->next != NULL)
            temp = temp->next;
        else
            break;
    }
    
    // SSN wrong length
    if (ssn < 100000000 || ssn > 999999999) {
        sprintf( buf_send, "103 " );
        printf( "\t SSN not of length 9: %d \n", ssn ); // Feedback
        return head;
    }
    
    // PIN wrong length
    if (pin < 1000 || pin > 9999) {
        sprintf(buf_send, "103 ");
        printf( "\t Pin not of length 4: %d \n", pin ); // Feedback
        return head;
    }
    
    // DL wrong length
    if (dl < 10000000 || dl > 99999999) {
        sprintf( buf_send, "103 " );
        printf("\t DL not of length 8: %d \n", dl); // Feedback
        return head;
    }
    
    struct Node* newNode = GetNewNode( fname, lname, pin, dl, ssn, email,head, buf_receive, buf_send);
    
    if(head == NULL)
        head = newNode;
    else
    {
        // Set newNode as head
        head->prev = newNode;
        newNode->next = head;
        head = newNode;
    }
    
    sprintf( buf_send, "104 " );
    printf("\t Added new Node \n" ); // Feedback
    
    return head;
}

/*---------------------------------------------------------------
 Authentication
 Command		<201 firstName PIN>
 Response	<203> Authentication failure
 <204> Authentication exceeded 10 failed attempts
 <205> Authentication success
 ---------------------------------------------------------------*/
int Authentication(char *fname,int pin,struct Node* head, char *buf_receive, char *buf_send)
{
    struct Node* temp = head;
    int result = 99;
    
    while(temp != NULL)
    {
        // Find fname
        result = strcmp( temp->fname, fname );
        if ( result == 0 )
        {
            // Match pin
            if( temp->pin == pin )
            {
                // Successful attempt (fname and pin are a match)
                sprintf( buf_send, "205 " );
                temp->FailedAttempts = 0; // Reset failed attempts
                printf( "\t Authenticated: %s %d \n", fname, pin ); // Feedback
                return temp->ssn;
            }
            else
            {
                // Failed attempt (incorrect pin for fname)
                temp->FailedAttempts = temp->FailedAttempts+1;
                if( temp->FailedAttempts >= 10 )
                {
                    // Exceeded login attempts
                    sprintf( buf_send, "204 " );
                    printf("\t Exceeded login attempts: %s %d - %d \n", fname, pin, temp->FailedAttempts ); // Feedback
                    temp->FailedAttempts = 0; // Reset failed attempts
                }
                else
                {
                    // Failed attempt
                    sprintf(buf_send, "203 ");
                    printf("\t Failed authentication: %s %d \n", fname, pin ); // Feedback
                }
                return -1;
            }
        }
        
        // Go to next Node
        if ( temp->next != NULL )
            temp = temp->next;
        else
            break;
    }
    
    // User not found
    sprintf(buf_send, "203 ");
    printf("\t Failed authentication, could not find: %s %d \n", fname, pin ); // Feedback
    return -1;
}

/*---------------------------------------------------------------
 Deposit
 Command		<301 Amount>
 <302> Machine full
 Response	<303 Balance> Deposit success
 <304 Balance> Deposit failed
 <305 Balance> Deposit return
 ---------------------------------------------------------------*/
int Deposit(float amount, int ssn,struct Node* head, char *buf_receive, char *buf_send){
    struct Node* temp = head;
    int result = 99,i=0, count=0;
    
    if (50000<amount+Dfund)
    {
        //Machine Full  302
        sprintf(buf_send, "302 %f ", temp->balance);;
        return 2;
    }
    
    
    while(temp != NULL)
    {
        if (temp->ssn == ssn && amount > 0)
        {
            temp->balance = temp->balance + amount;
            
            for (i=0; i<5; i++)
            {
                
                result = strcmp(temp->transcations[i],"null");
                if (result==0)
                {
                    temp->count=i;
                    strcpy(temp->transcations[i],"Deposit");
                    temp->tamount[i] = amount;
                    count++;
                    break;
                }
            }
            
            if (count==0)
            {
                temp->count=temp->count+1;
                strcpy(temp->transcations[(temp->count)%5],"Deposit");
                temp->tamount[(temp->count)%5] = amount;
            }
            
            //succesful deposit 303, balance
            sprintf(buf_send, "303 %f ", temp->balance);
            return 0;
        }
        if (temp->next != NULL)
        {
            temp = temp->next;
        }
        else
            break;
    }
    
    //Failed deposit 304, balance
    sprintf(buf_send, "304 %f ", temp->balance);;
    return 1;
}

/*---------------------------------------------------------------
 Withdraw
 Command		<401 Amount>
 <402> Empty (ATM doesn't have enough money to make withdrawl)
 Response	<403 Balance> Withdraw success
 <404 Balance> Funds low
 <405> Withdraw return
 ---------------------------------------------------------------*/
//Withdraw(
int Withdraw(float amount, int ssn,struct Node* head, char *buf_receive, char *buf_send){
    struct Node* temp = head;
    int result = 99,i=0, count=0;
    
    
    if (amount>fund)
    {
        //Machine Full  302
        sprintf(buf_send, "402 %f ", temp->balance);;
        return 2;
    }
    
    while(temp != NULL)
    {
        if (temp->ssn == ssn && amount > 0)
        {
            if (temp->balance < amount)
            {
                sprintf(buf_send, "404 %f ", temp->balance);
                //printf("Withdraw Failed. Not enough funds. Balance is: %f\n", temp->balance);
                return 2;
            }
            
            temp->balance = temp->balance - amount;
            
            
            for (i=0; i<5; i++)
            {
                result = strcmp(temp->transcations[i],"null");
                if (result==0)
                {
                    temp->count=i;
                    strcpy(temp->transcations[i],"Withdraw");
                    temp->tamount[i] = amount;
                    count++;
                    break;
                }
            }
            
            if (count==0)
            {
                temp->count=temp->count+1;
                strcpy(temp->transcations[(temp->count)%5],"Withdraw");
                temp->tamount[(temp->count)%5] = amount;
            }
            
            sprintf(buf_send, "403 %f ", temp->balance);
            amount=amount-fund;
            //printf("Withdraw successful. New balance is: %f\n", temp->balance);
            return 0;
        }
        if (temp->next != NULL)
        {
            temp = temp->next;
        }
        else
            break;
    }
    
    sprintf(buf_send, "405 ");
    //printf("Withdraw Failed. Balance is: %f\n", temp->balance);
    return 1;
}


/*---------------------------------------------------------------
 Balance
 Command		<501>
 Response	<503 Balance> Return user balance
 ---------------------------------------------------------------*/
int Balance(int ssn,struct Node* head, char *buf_receive, char *buf_send){
    struct Node* temp = head;
    
    while(temp != NULL)
    {
        if (temp->ssn == ssn)
        {
            sprintf(buf_send, "503 %f\n", temp->balance);
            //printf("Balance is: %f\n", temp->balance);
            return 0;
        }
        if (temp->next != NULL)
        {
            temp = temp->next;
        }
        else
            break;
    }
    
    sprintf(buf_send, "Balance Failed.");
    //printf("Balance Failed.");
    return 1;
}

/*---------------------------------------------------------------
 Show Transactions
 Command		<601 numOfTransactions>
 Response	<603 TransactionCount TransactionMessage> Return transactions
 ---------------------------------------------------------------*/
int ShowTransactions(int ssn, int n,struct Node* head, char *buf_receive, char *buf_send){
    struct Node* temp = head;
    int result = 99,i=0, count=0;
    char test[80+1];
    if (n>5)
    {
        
        sprintf(buf_send, "Only 5 transcation are kept.\n");
        //printf("Only 5 transcation are kept.\n");
        return 2;
    }
    
    while(temp != NULL)
    {
        if (temp->ssn == ssn)
        {
            sprintf(buf_send, "603 ");
            for (i=temp->count; i>(temp->count-n); i--)
            {
                result = temp->tamount[i%5];
                if (result>0)
                {
                    count ++;
                    //printf("Result: %d ", result);
                    sprintf(buf_receive, "%d %f %s\n", count,temp->tamount[i%5], temp->transcations[i%5]);
                    strcat(buf_send,buf_receive);
                    //printf("Tranaction: %s Amount: %f\n", temp->transcations[i%5],temp->tamount[i%5]);
                }
            }
            sprintf(buf_receive, ";");
            return 0;
        }
        if (temp->next != NULL)
        {
            temp = temp->next;
        }
        else
        {
            break;
        }
    }
    
    strcat(buf_send,"Balance Failed.\n");
    printf("Balance Failed.\n");
    return 1;
}


/*---------------------------------------------------------------
 Stamps
 Command		<701 Amount>
 <702> Out of stamps
 Response	<703> Terminate transaction
 <704 Balance> Success
 <705> Stamps return
 ---------------------------------------------------------------*/
int Stamps(float amount, int ssn,struct Node* head, char *buf_receive, char *buf_send){
    struct Node* temp = head;
    int result = 99,i=0, count=0;
    
    while(temp != NULL)
    {
        if (temp->ssn == ssn && amount > 0)
        {
            if (stamp < amount)
            {
                sprintf(buf_send, "702 ");
                return 2;
            }
            
            
            if (temp->balance < amount)
            {
                sprintf(buf_send, "703 ");
                //printf("Buy Stamps Failed. Not enough funds. Balance is: %f\n", temp->balance);
                return 2;
            }
            
            
            temp->balance = temp->balance - amount;
            stamp = stamp - amount;
            
            for (i=0; i<5; i++)
            {
                result = strcmp(temp->transcations[i],"null");
                if (result==0)
                {
                    temp->count=i;
                    strcpy(temp->transcations[i],"Stamps");
                    temp->tamount[i] = amount;
                    count++;
                    break;
                }
            }
            
            if (count==0)
            {
                temp->count=temp->count+1;
                strcpy(temp->transcations[(temp->count)%5],"Stamps");
                temp->tamount[(temp->count)%5] = amount;
            }
            
            
            sprintf(buf_send, "704 %f ", temp->balance);
            //printf("Successful bought stamps. New balance is: %f\n", temp->balance);
            return 0;
        }
        if (temp->next != NULL)
        {
            temp = temp->next;
        }
        else
            break;
    }
    
    sprintf(buf_send, "Failed to buy stamps. Balance is: %f\n", temp->balance);
    //printf("Failed to buy stamps. Balance is: %f\n", temp->balance);
    return 1;
}


/*---------------------------------------------------------------
 Logout
 Command		<801> 
 Response	<803> Logout success
 ---------------------------------------------------------------*/
void Logout(struct Node* head, char *buf_receive, char *buf_send) {
    printf("Logout\n");
    sprintf(buf_send, "803 ");
}



