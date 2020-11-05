/*********************************************************************//**
 * @file setup.h
 * @brief Setup routine prototypes.
 **********************************************************************/

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
