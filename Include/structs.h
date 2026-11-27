#ifndef TYPES_H
#define TYPES_H

#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// were using these as global constants to avoid hardcoding
// values later on (we might add more later on i think)
#define BOARD_SLOTS 4
#define MAX_HAND 10
#define MAX_DECK 30
#define MAX_NAME 32

// screen
typedef enum {
    SCREEN_MENU,
    SCREEN_GAMEPLAY,
    SCREEN_BATTLE,
    SCREEN_SHOP,
    SCREEN_CAMP,
    SCREEN_EVENT,
    SCREEN_MAP
} GameScreen;

// the 3 types of cards
typedef enum {
    CARD_ATTACK,
    CARD_SHIELD,
    CARD_EFFECT
} CardType;

// status effect categories
typedef enum {
    STATUS_NONE = 0,
    STATUS_BUFF,
    STATUS_DEBUFF
} StatusID;

// represents an individual active status effect
typedef struct {
    StatusID id;
    int duration;
    int value;
} StatusEffect;

// the struct for our CARDS this one holds the stats for
// each unique card that we will be coming up with
typedef struct {
    char name[MAX_NAME];
    CardType type;
    int power;
    int rarity;
    int is_once_per_encounter;
    int is_active;
    int price;
} Card;

// fighter struct containing dedicated buff and debuff slots
typedef struct {
    char name[MAX_NAME];
    int max_hp;
    int current_hp;
    int shield;
    StatusEffect active_buff;
    StatusEffect active_debuff;
} Fighter;

// this struct is for the main character (MC) and holds the player fighter
typedef struct {
    Fighter player;
} MC;

// this struct represents card collections
typedef struct {
    Card card[MAX_DECK];
    int count;
} Deck;

// this struct represents the state of a battle, including player and enemy fighters, card slots, draw and discard piles, hand, round number, and next round draw count
typedef struct {
    Fighter player;
    Fighter enemy;
    Card player_slots[BOARD_SLOTS];
    Card enemy_slots[BOARD_SLOTS];
    Deck draw_pile;
    Deck discard_pile;
    Card hand[MAX_HAND];
    int hand_count;
    int round_number;
    int next_round_draw;
} Battle;

#endif