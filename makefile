all: blackjack

blackjack: blackjack.c
	gcc -std=c18 -g -o blackjack blackjack.c
