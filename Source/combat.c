#include "../Include/structs_&&_functions.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

//this body of code until line 17 is a helper function to create an empty card with default values,
//used for clearing board slots and initializing card arrays.
static Card create_empty_card(void) {
    Card empty_card;
    memset(&empty_card, 0, sizeof(Card));
    strcpy(empty_card.name, "EMPTY");
    empty_card.type = CARD_EFFECT;
    empty_card.power = 0;
    empty_card.rarity = 0;
    empty_card.is_once_per_encounter = 0;
    empty_card.is_active = 0;
    empty_card.price = 0;
    return empty_card;
}

//this function is in charge of applying damage to a fighter,
//taking into account their shield and current HP.
//It ensures that damage is absorbed by the shield first,
//and any remaining damage reduces the fighter's HP.
//If the fighter's HP drops below zero, it is set to zero to avoid negative values.
static void apply_damage_to_fighter(Fighter *target, int damage_amount) {
    if (damage_amount <= 0) return;

    if (target->shield >= damage_amount) {
        target->shield -= damage_amount;
    } else {
        int remaining_damage = damage_amount - target->shield;
        target->shield = 0;
        target->current_hp -= remaining_damage;
        if (target->current_hp < 0) {
            target->current_hp = 0;
        }
    }
}

// this monstrous looking function initializes the battle state,
//setting up both the player and enemy fighters,
// clearing board slots, copying the master deck into the draw pile,
//shuffling it, and drawing the initial hand of cards.
void initializeBattle(Battle *battle, int startingHP, const Deck *masterDeck, Fighter enemyData) {
    // Initialize Player Stats
    strcpy(battle->player.name, "KuroKami Mikami");
    battle->player.max_hp = 100;
    battle->player.current_hp = startingHP;
    battle->player.shield = 0;
    battle->player.active_buff = (StatusEffect){STATUS_NONE, 0, 0};
    battle->player.active_debuff = (StatusEffect){STATUS_NONE, 0, 0};

    // intializes enemy stats from the provided enemyData struct
    battle->enemy = enemyData;
    battle->enemy.shield = 0;
    battle->enemy.active_buff = (StatusEffect){STATUS_NONE, 0, 0};
    battle->enemy.active_debuff = (StatusEffect){STATUS_NONE, 0, 0};

    // clears all board slots for both player and enemy to ensure a fresh start
    for (int i = 0; i < BOARD_SLOTS; i++) {
        battle->player_slots[i] = create_empty_card();
        battle->enemy_slots[i] = create_empty_card();
    }

    // copies the master deck into the battle's draw pile and marks all cards as active,
    // then initializes the discard pile and hand count to zero, and sets the round number and
    battle->draw_pile = *masterDeck;
    for (int i = 0; i < battle->draw_pile.count; i++) {
        battle->draw_pile.card[i].is_active = 1;
    }
    battle->discard_pile.count = 0;
    battle->hand_count = 0;
    battle->round_number = 1;
    battle->next_round_draw = 0;

    // shufffles the draw pile to randomize card order
    //and draws the initial hand of 5 cards for the player,
    // then generates the enemy's move for the first round.
    shuffleDeck(&battle->draw_pile);
    drawCards(battle, 5);
    generateEnemyMove(battle);
}

// calculates how many cards the player should draw next round based on the card they played this turn.
int calculateNextDraw(const Card playedCard) {
    if (!playedCard.is_active || strcmp(playedCard.name, "EMPTY") == 0) {
        return 0;
    }
    if (playedCard.is_once_per_encounter) {
        return 3;
    }
    if (playedCard.type == CARD_EFFECT) {
        return 2;
    }
    if (playedCard.type == CARD_SHIELD) {
        return 1;
    }
    return 0; // CARD_ATTACK grants 0 next-round draw
}

// the purpoe of this function is to stage a card from the player's hand onto an open board slot.
// then it checks if the hand index and slot index are valid, ensures the target slot is empty,
// and then moves the card from the hand to the board slot, marking it as active.
int playCardToSlot(Battle *battle, int handIndex, int slotIndex) {
    if (handIndex < 0 || handIndex >= battle->hand_count) return 0;
    if (slotIndex < 0 || slotIndex >= BOARD_SLOTS) return 0;

    // checks if target slot is already occupied, if so, it returns 0 to indicate failure.
    if (battle->player_slots[slotIndex].is_active) {
        return 0;
    }

    // moves the card from the hand to the specified board slot and marks it as active.
    battle->player_slots[slotIndex] = battle->hand[handIndex];
    battle->player_slots[slotIndex].is_active = 1;

    // Shifts remaining cards in hand to fill the gap left by the played card and decrements the hand count.
    for (int i = handIndex; i < battle->hand_count - 1; i++) {
        battle->hand[i] = battle->hand[i + 1];
    }
    battle->hand_count--;

    return 1;
}

// this hellish looking code that took me HOURS to write is responsible for retracting a card from the player's board slot back into their hand.
// it checks if the slot index is valid, ensures the slot contains an active card, and verifies that the player's hand isn't already full.
int retractCardFromSlot(Battle *battle, int slotIndex) {
    if (slotIndex < 0 || slotIndex >= BOARD_SLOTS) return 0;
    if (!battle->player_slots[slotIndex].is_active) return 0;
    if (battle->hand_count >= MAX_HAND) return 0;

    // Return card to hand
    battle->hand[battle->hand_count] = battle->player_slots[slotIndex];
    battle->hand_count++;

    // Clear board slot
    battle->player_slots[slotIndex] = create_empty_card();

    return 1;
}

// added 11/22/26: taints a card in the player's hand, sending it to the discard pile and drawing 4 new cards
int taintCard(Battle *battle, int handIndex) {
    if (handIndex < 0 || handIndex >= battle->hand_count) return 0;

    // creates the Tarnished / Overclock card
    Card tarnished_card;
    memset(&tarnished_card, 0, sizeof(Card));
    strcpy(tarnished_card.name, "Tarnished");
    tarnished_card.type = CARD_EFFECT;
    tarnished_card.power = 0;
    tarnished_card.rarity = 1;
    tarnished_card.is_once_per_encounter = 0; 
    tarnished_card.is_active = 0;             
    tarnished_card.price = 0;

    // sends the tarnished card to the discard pile
    if (battle->discard_pile.count < MAX_DECK) {
        battle->discard_pile.card[battle->discard_pile.count] = tarnished_card;
        battle->discard_pile.count++;
    }

    // shifts remaining hand cards to fill the gap
    for (int i = handIndex; i < battle->hand_count - 1; i++) {
        battle->hand[i] = battle->hand[i + 1];
    }
    battle->hand_count--;

    // draws 4 cards as reward/penalty for tainting
    drawCards(battle, 4);

    return 1;
}

// this function generates the enemy's move for the current round
// by randomly selecting two distinct board slots and assigning them 
// either an attack or shield card based on the round number.
void generateEnemyMove(Battle *battle) {
    // Clear existing enemy slots first
    for (int i = 0; i < BOARD_SLOTS; i++) {
        battle->enemy_slots[i] = create_empty_card();
    }

    // this is a simple example of enemy behavior: 
    // it will always play an attack card in one slot
    // and either an attack or shield in the other slot based on the round number.
    Card enemy_attack;
    memset(&enemy_attack, 0, sizeof(Card));
    strcpy(enemy_attack.name, "Fireball");
    enemy_attack.type = CARD_ATTACK;
    enemy_attack.power = 6;
    enemy_attack.rarity = 1;
    enemy_attack.is_active = 1;

    Card enemy_guard;
    memset(&enemy_guard, 0, sizeof(Card));
    strcpy(enemy_guard.name, "Arcane Ward");
    enemy_guard.type = CARD_SHIELD;
    enemy_guard.power = 5;
    enemy_guard.rarity = 1;
    enemy_guard.is_active = 1;

    // picks 2 distinct random slots (BOARD_SLOTS / 2)
    int slot_1 = rand() % BOARD_SLOTS;
    int slot_2 = (slot_1 + 1 + (rand() % (BOARD_SLOTS - 1))) % BOARD_SLOTS;

    battle->enemy_slots[slot_1] = enemy_attack;
    battle->enemy_slots[slot_2] = (battle->round_number % 2 == 1) ? enemy_attack : enemy_guard;
}

// this functions prpose is to resolve the combat turn by
// processing each player's and enemy's card in their respective board slots.
void resolveCombatTurn(Battle *battle) {
    battle->next_round_draw = 0;

    for (int i = 0; i < BOARD_SLOTS; i++) {
        // 1. resolves Player Card in Slot i
        Card *p_card = &battle->player_slots[i];
        if (p_card->is_active) {
            if (p_card->type == CARD_ATTACK) {
                apply_damage_to_fighter(&battle->enemy, p_card->power);
            } else if (p_card->type == CARD_SHIELD) {
                battle->player.shield += p_card->power;
            } else if (p_card->type == CARD_EFFECT) {
                // handles self-heal vs magic/utility effects
                if (strstr(p_card->name, "Heal") != NULL || strstr(p_card->name, "Superguard") != NULL) {
                    battle->player.current_hp += p_card->power;
                    if (battle->player.current_hp > battle->player.max_hp) {
                        battle->player.current_hp = battle->player.max_hp;
                    }
                } else {
                    apply_damage_to_fighter(&battle->enemy, p_card->power);
                }
            }
            battle->next_round_draw += calculateNextDraw(*p_card);
        }

        // Stop mid-turn if enemy is already defeated
        if (checkBattleEnd(battle) != 0) return;

        // 2. Resolve Enemy Card in Slot i
        Card *e_card = &battle->enemy_slots[i];
        if (e_card->is_active) {
            if (e_card->type == CARD_ATTACK) {
                apply_damage_to_fighter(&battle->player, e_card->power);
            } else if (e_card->type == CARD_SHIELD) {
                battle->enemy.shield += e_card->power;
            } else if (e_card->type == CARD_EFFECT) {
                battle->enemy.current_hp += e_card->power;
                if (battle->enemy.current_hp > battle->enemy.max_hp) {
                    battle->enemy.current_hp = battle->enemy.max_hp;
                }
            }
        }

        // Stop mid-turn if player is defeated
        if (checkBattleEnd(battle) != 0) return;
    }
}

//added 11/22/26: this function processes the active buffs and debuffs for a given fighter,
//applying their effects and decrementing their durations. in simple terms, it handles the ongoing effects of status conditions like regeneration or poison.
static void process_fighter_statuses(Fighter *fighter) {
    // processes Active Buffs (e.g., Regeneration)
    if (fighter->active_buff.id != STATUS_NONE && fighter->active_buff.duration > 0) {
        if (fighter->active_buff.value > 0) {
            fighter->current_hp += fighter->active_buff.value;
            if (fighter->current_hp > fighter->max_hp) {
                fighter->current_hp = fighter->max_hp;
            }
        }
        fighter->active_buff.duration--;
        if (fighter->active_buff.duration <= 0) {
            fighter->active_buff.id = STATUS_NONE;
            fighter->active_buff.duration = 0;
            fighter->active_buff.value = 0;
        }
    }

    // processes Active Debuffs (e.g., Burning, Poisoned, Bleeding)
    if (fighter->active_debuff.id != STATUS_NONE && fighter->active_debuff.duration > 0) {
        if (fighter->active_debuff.value > 0) {
            fighter->current_hp -= fighter->active_debuff.value;
            if (fighter->current_hp < 0) {
                fighter->current_hp = 0;
            }
        }
        fighter->active_debuff.duration--;
        if (fighter->active_debuff.duration <= 0) {
            fighter->active_debuff.id = STATUS_NONE;
            fighter->active_debuff.duration = 0;
            fighter->active_debuff.value = 0;
        }
    }
}

// Evaluates active buff and debuff slots for both player and enemy and decrements duration counters
void applyStatusEffects(Battle *battle) {
    process_fighter_statuses(&battle->player);
    process_fighter_statuses(&battle->enemy);
}

// this function advances the battle to the next round by discarding spent cards,
// exiling single-use cards, and drawing new cards for the player.
void advanceRound(Battle *battle) {
    // just added this 11/23/26: didnt realize this was causing the problems cuz i didnt call it in the first place,
    //but now it should apply buffs/debuffs at the start of each round
    applyStatusEffects(battle);
    // moves spent player cards to discard pile or exile if once-per-encounter
    for (int i = 0; i < BOARD_SLOTS; i++) {
        if (battle->player_slots[i].is_active) {
            if (!battle->player_slots[i].is_once_per_encounter) {
                if (battle->discard_pile.count < MAX_DECK) {
                    battle->discard_pile.card[battle->discard_pile.count] = battle->player_slots[i];
                    battle->discard_pile.count++;
                }
            }
            // resets slot
            battle->player_slots[i] = create_empty_card();
        }
    }

    // guarantees at least 1 card draw if player only played attacks and hand is empty
    int cards_to_draw = battle->next_round_draw;
    if (cards_to_draw == 0 && battle->hand_count == 0) {
        cards_to_draw = 2;
    }

    drawCards(battle, cards_to_draw);
    battle->round_number++;
    generateEnemyMove(battle);
}

// this function checks the battle state to determine
// if the player has won, lost, or if the battle is still ongoing.
int checkBattleEnd(const Battle *battle) {
    if (battle->enemy.current_hp <= 0) {
        return 1; // player victory
    }
    if (battle->player.current_hp <= 0) {
        return 2; // player defeat
    }
    return 0;     // ongoing
}