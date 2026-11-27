#ifndef DATABASE_H
#define DATABASE_H

#include "structs.h"

/* loads card definitions, types, and base stats from an external file */
void loadCardDatabase(Card database[], const char *filename);

#endif