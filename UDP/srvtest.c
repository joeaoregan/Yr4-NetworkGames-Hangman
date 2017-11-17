// 20171116 CreateUDPSocket.h - abstracts the UDP socket creation code
// 20171115 Fixed client continuing to connect
// 20171114 Joe: Fixed warning messages by casting sockaddr_in to struct sockaddr*
// 		 And moved HandleErrors to parent folder	
// 		Added random word selection	

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>					// Seed the random number
#include "../HandleErrors.h"				// Moved HandleErrors.h to parent folder
#include "../Hangman.h"
#include "../DrawHangman.h"
#include "../CreateUDPSocket.h"

//struct sockaddr_in srvAddr;
struct sockaddr_in cliAddr;

int slen = sizeof(cliAddr);

int main(void) {
	int sock;
	char buf[LINESIZE];

	sock = createUDPServer();

	drawHangman();

	printf("\nWaiting For Data...\n");

	//while(1) {

		fflush(stdout);
		srand(time(NULL));

		play_hangman(sock,sock);

//		printf("Finished Play Hangman\n");	
	//}

	close(sock);	

	return 0;
}

/* ---------------- Play_hangman () ---------------------*/
void play_hangman (int in, int out) {
	printf("\nBegin Playing\n");

 	char * whole_word, part_word[LINESIZE], guess[LINESIZE], outbuf[LINESIZE];
 	int lives = MAX_LIVES, game_state = 'I', i, good_guess, word_length;
 	char hostname[LINESIZE];
	ssize_t byteCount;

 //	gethostname (hostname, LINESIZE);
//	sprintf(outbuf, "Playing hangman on host %s: \n \n", hostname);			
 //	write(0, outbuf, strlen (outbuf));					// XXX	 	
/*RECEIVE USERNAME*/
	if((byteCount = recvfrom(in, guess, LINESIZE, 0, (struct sockaddr *) &cliAddr, &slen)) == 0) {	// Server receives 1st
		displayErrMsg("recvfrom() Failed");
	}		
	printf("byteCount: %ld\n", byteCount);
	//guess[byteCount-1] = '\0';
	printf("Username received: %s\n",guess);
/*GET RANDOM WORD*/
	whole_word = selectRandomWord("Client", 0000);				// XXX FIX THIS LATER - CLIENT IP AND PORT
 	word_length = strlen(whole_word);

 	
 	for (i = 0; i < word_length; i++) {
		part_word[i] = '-';
	}
	part_word[i] = '\0';
	guess[0] = '\0';

//	printf("test 2, recv username, random word selected\n");		// XXX

 
	part_word[i] = '\0';
 	sprintf (outbuf, "%s %d \n", part_word, lives);	
	sendto(out, outbuf, strlen(outbuf), 0, (struct sockaddr*) &cliAddr, sizeof cliAddr);	

	while (game_state == 'I') {	
		guess[0] = '\0';
		if (byteCount = recvfrom(in, guess, LINESIZE, 0, (struct sockaddr *) &cliAddr, &slen) == -1){	// cast sockaddr_in to sockaddr
			 displayErrMsg("recvfrom() Failed");
		}
		printf("byteCount: %ld\n", byteCount);
		guess[byteCount-2] = '\0';

		printf("Client Sent: %s\n", guess);

 		good_guess = 0;

 		for (i = 0; i < word_length; i++) {			
 			if (guess [0] == whole_word[i]) {				
 				good_guess = 1;						
 				part_word [i] = whole_word[i];			
 			}
 		}

 		if (!good_guess) lives--;	

 		if (strcmp (whole_word, part_word) == 0) game_state = 'W';

 		else if (lives == 0)  {
 			game_state = 'L'; 						
 			strcpy (part_word, whole_word); 
 		}

 		sprintf (outbuf, "%s %d \n", part_word, lives);	
		sendto(out, outbuf, LINESIZE, 0, (struct sockaddr *) &cliAddr, slen);			// cast sockaddr_in to sockaddr

//		printf("Game State %c\n", game_state);
	}	
	
	// Send x to exit
	sprintf (outbuf, "%s\n", "bye");	
	sendto(out, outbuf, LINESIZE, 0, (struct sockaddr *) &cliAddr, slen);			// cast sockaddr_in to sockaddr
	close(in);
}