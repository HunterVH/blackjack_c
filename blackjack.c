/*
 * Author: Hunter Van Horn
 * Date: 04-01-2025
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>

#ifdef _WIN32
	#define CLEARSCREEN "cls"
#else
	#define CLEARSCREEN "clear"
#endif

#define DEALERHIT 16

enum suit{
	spade = 0,
	diamond = 1,
	club = 2,
	heart = 3
};

struct card{
	enum suit cardSuit;
	short cardType;
	short cardValue;
};

struct play{
	struct card* hand;
	short hasAce;
	short handSize;
	short handValue;
	short hasBusted;
};

typedef struct play* player;


void shuffleDeck(struct card crd[], int variety){
	srand(time(NULL));
	for(int i=0; i<variety; i++){
		int position1 = rand()%52;
		int position2 = rand()%52;
		struct card temp = crd[position1];
		crd[position1] = crd[position2];
		crd[position2] = temp;
	}
}

void printCard(struct card crd){
	switch(crd.cardType){
		case 0:
			printf("J");
			break;
		case 1:
			printf("A");
			break;
		case 11:
			printf("Q");
			break;
		case 12:
			printf("K");
			break;
		default:
			printf("%d", crd.cardValue);
	}
	printf("%s", crd.cardSuit == spade ? "s" : crd.cardSuit == diamond ? "d" : crd.cardSuit == club ? "c" : "h");
}

struct card* createDeck(void){
	struct card* deck = (struct card*)malloc(52*sizeof(struct card));
	for(int i=0; i<52; i++){
		switch(i/13){
			case spade:
				deck[i].cardSuit = spade;
				break;
			case diamond:
				deck[i].cardSuit = diamond;
				break;
			case club:
				deck[i].cardSuit = club;
				break;
			case heart:
				deck[i].cardSuit = heart;
				break;
			default:
				deck[i].cardSuit = -1;
		}
		switch(i%13){
			case 0:
				deck[i].cardValue = 10;
				break;
			case 1:
				deck[i].cardValue = 1;
				break;
			case 11:
				deck[i].cardValue = 10;
				break;
			case 12:
				deck[i].cardValue = 10;
				break;
			default:
				deck[i].cardValue = i%13;
		}
		deck[i].cardType = i%13;
	}
	return deck;
}

void hit(struct card* deck, int* deckTracker, player player1){
	// If the whole deck has been reused shuffle and start from the beginning
	if(*deckTracker == 52){
		shuffleDeck(deck, 250);
		*deckTracker = 0;
	}
	player1->hand[player1->handSize] = deck[*deckTracker];
	if(deck[*deckTracker].cardValue == 1){
		player1->hasAce = 1;
	}
	player1->handValue += player1->hand[player1->handSize].cardValue;
	player1->handSize++;
	*deckTracker += 1;
	if(player1->handValue > 21){
		player1->hasBusted = 1;
	}
}

void initialDeal(struct card* deck, int* deckTracker, player player1, player dealer){
	hit(deck, deckTracker, player1);
	hit(deck, deckTracker, dealer);
	hit(deck, deckTracker, player1);
	hit(deck, deckTracker, dealer);
}

void printPlayerHand(player player1, short isDealer){
	if(isDealer){
		printCard(player1->hand[0]);
		printf(", *");
		printf("\n\tTotal: ");
		if(player1->hand[0].cardValue == 1){
			printf("%d, ", player1->hand[0].cardValue+10, player1->hand[0].cardValue);
		}
		printf("%d\n", player1->hand[0].cardValue);
	}
	else{
		for(int i=0; i<player1->handSize; i++){
			printCard(player1->hand[i]);
			if(i<(player1->handSize)-1){
				printf(", ");
			}
		}
		printf("\n\tTotal: ");
		if(player1->hasBusted){
			printf("BUSTED\n");
		}
		else if(player1->hasAce){
			if((player1->handValue)+10 <= 21)
				printf("%d, %d\n", player1->handValue+10, (player1->handValue));
			else
				printf("%d\n", player1->handValue);
		}
		else{
			printf("%d\n", player1->handValue);
		}
	}
}

void determineWinner(player player1, player dealer){
	printf("Player:\n");
	printPlayerHand(player1, 0);
	printf("\nDealer:\n");
	printPlayerHand(dealer, 0);

	printf("\n");

	short dealHandValue;
	short playHandValue;

	if(player1->hasAce){
		playHandValue = (player1->handValue)+10 <= 21 ? player1->handValue+10 : player1->handValue;
	}
	else{
		playHandValue = player1->handValue;
	}

	if(dealer->hasAce){
		dealHandValue = (dealer->handValue)+10 <= 21 ? dealer->handValue+10 : dealer->handValue;
	}
	else{
		dealHandValue = dealer->handValue;
	}


	if(player1->hasBusted || dealer->hasBusted){
		if(player1->hasBusted && dealer->hasBusted){
			printf("A Push, No Winner\n");
		}
		else if(player1->hasBusted){
			printf("Dealer Wins\n");
		}
		else{
			printf("Player Wins\n");
		}
	}
	else{
		if(playHandValue == dealHandValue){
			printf("A Push, No Winner\n");
		}
		else if(playHandValue > dealHandValue){
			printf("Player Wins\n");
		}
		else{
			printf("Dealer Wins\n");
		}
	}
	
	player1->handValue = 0;
	player1->hasAce = 0;
	player1->handSize = 0;
	player1->hasBusted = 0;

	dealer->handValue = 0;
	dealer->hasAce = 0;
	dealer->handSize = 0;
	dealer->hasBusted = 0;
}

void wait(short time){
	sleep(time);
	system(CLEARSCREEN);
}

void dealerPlays(player player1, player dealer, struct card* deck, int* deckTracker){
	printf("Player:\n");
	printPlayerHand(player1, 0);
	printf("\nDealer:\n");
	printPlayerHand(dealer, 0);
	short handValue;
	if(dealer->hasAce){
		handValue = (dealer->handValue)+10 <= 21 ? dealer->handValue+10 : dealer->handValue;
	}
	else{
		handValue = dealer->handValue;
	}
	while(handValue <= DEALERHIT){
		sleep(1);
		printf("Dealer Hits\n");
		wait(2);
		hit(deck, deckTracker, dealer);
		printf("Player:\n");
		printPlayerHand(player1, 0);
		printf("\nDealer:\n");
		printPlayerHand(dealer, 0);
		if(dealer->hasAce){
			handValue = (dealer->handValue)+10 <= 21 ? dealer->handValue+10 : dealer->handValue;
		}
		else{
			handValue = dealer->handValue;
		}
	}
	sleep(1);
	if(dealer->hasBusted)
		printf("Dealer Busted\n");
	else
		printf("Dealer Stays\n");
	wait(2);
}

int main(void){
	int tracker = 0;
	int playing = 1;
	int response = 1;
	char userInput[32];
	struct card* deck = createDeck();
	player player1 = (player)malloc(sizeof(struct play));
	player1->hand = (struct card*)malloc(11*sizeof(struct card));
	player1->handSize = 0;
	player1->hasAce = 0;
	player1->handValue = 0;
	player1->hasBusted = 0;
	player dealer = (player)malloc(sizeof(struct play));
	dealer->hand = (struct card*)malloc(11*sizeof(struct card));
	dealer->handSize = 0;
	dealer->hasAce = 0;
	dealer->handValue = 0;
	dealer->hasBusted = 0;

	shuffleDeck(deck, 104);
	while(playing){
		initialDeal(deck, &tracker, player1, dealer);
		wait(0);
		while(response){
			printf("Player:\n");
			printPlayerHand(player1, 0);
			printf("\nDealer:\n");
			printPlayerHand(dealer, 1);
			printf("\n(H)it, (S)tay, or (Q)uit\n");
			fgets(userInput, 32, stdin);
			if(toupper(userInput[0]) == 'H'){
				printf("Hit.\n");
				hit(deck, &tracker, player1);
				if(player1->hasBusted){
					response = 0;
				}
				else
					response = 1;
			}
			else if(toupper(userInput[0]) == 'S'){
				printf("Stay.\n");
				response = 0;
			}
			else if(toupper(userInput[0]) == 'Q'){
				printf("Quitter.\n");
				response = 0;
				playing = 0;
			}
			else{
				printf("That is not a valid input\n");
			}
			if(playing)
				wait(1);
		}
		if(playing){
			dealerPlays(player1, dealer, deck, &tracker);
			determineWinner(player1, dealer);
			wait(2);
		}
		response = 1;
	}
	
	free(player1->hand);
	free(player1);
	free(dealer->hand);
	free(dealer);
	free(deck);
	return 0;
}
