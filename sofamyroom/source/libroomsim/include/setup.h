/*********************************************************************//**
 * @file setup.h
 * @brief Setup routine prototypes.
 **********************************************************************//*
 * Author: Steven Schimmel, stevenmschimmel@gmail.com
 * Copyright 2009, University of Zurich
 *************************************************************************
 * This file is part of ROOMSIM.
 *
 * ROOMSIM is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version. 
 *
 * ROOMSIM is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ROOMSIM. If not, see <http://www.gnu.org/licenses/>.
 *************************************************************************/

#ifndef SETUP_H_2193798154871641379127
#define SETUP_H_2193798154871641379127

struct CFileSetupItem {
	enum {SI_FIELD,SI_STRUCT} type;
	char *name;
	union {
		char *value;
		struct CFileSetupItem *field;
	} data;
	struct CFileSetupItem *next, *prev, *parent;
};
typedef struct CFileSetupItem CFileSetupItem;

typedef struct {
	CFileSetupItem	root;
	unsigned long	buflen;
	char			*buf;
} CFileSetup;

int ReadSetup(char *filename, CFileSetup *pSetup);
void PrintSetup(CFileSetupItem *item);
void SetupPrintItem(CFileSetupItem *item);
void SetupPrintItemName(CFileSetupItem *item);


CFileSetupItem *SetupFindField(CFileSetupItem *item, char *name);
CFileSetupItem *SetupFindStruct(CFileSetupItem *item, char *name);

#endif /* SETUP_H_2193798154871641379127 */
