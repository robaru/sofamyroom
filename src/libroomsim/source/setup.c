/*********************************************************************//**
 * @file setup.c
 * @brief Setup routines.
 **********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "setup.h"
#include "defs.h"

/* disable warnings about unsafe CRT functions */
#ifdef _MSC_VER
#  pragma warning( disable : 4996)
#endif

/** @todo: accept (single/double) quotes around strings */
/** @todo: accept semicolons after values */
/** @todo: accept comments (marked by %) */
/** @todo: accept (1) array indexing */

CFileSetupItem *SetupFindStruct(CFileSetupItem *item, char *name)
{
	if (!item || item->type != SI_STRUCT) 
		return NULL;

	item = item->data.field;
	while (item)
	{
		if (item->type == SI_STRUCT && stricmp(item->name, name)==0)
			break;
		item = item->next;
	}

	return item;
}

CFileSetupItem *SetupFindField(CFileSetupItem *item, char *name)
{
	if (!item || item->type != SI_STRUCT) 
		return NULL;

	item = item->data.field;
	while (item)
	{
		if (item->type == SI_FIELD && stricmp(item->name, name)==0)
			break;
		item = item->next;
	}

	return item;
}

void SetupPrintItem(CFileSetupItem *item)
{
	/* don't print invalid items */
	if (!item)			return;
	/* print parent first */
	if (item->parent)	SetupPrintItem(item->parent);
	/* don't print root item */
	if (!item->parent)	return;

	/* print field */
	if (item->type == SI_FIELD)
		printf("%s = %s\n", item->name, item->data.value);
	else if (item->data.field)
		printf("%s.", item->name);
	else
		printf("%s\n", item->name);
}

void SetupPrintItemName(CFileSetupItem *item)
{
	/* don't print invalid items */
	if (!item)			return;
	/* print parent first */
	if (item->parent)	SetupPrintItemName(item->parent);
	/* don't print root item */
	if (!item->parent)	return;

	/* print field */
	if (item->type == SI_FIELD)
		printf("%s", item->name);
	else
		printf("%s.", item->name);
}

static CFileSetupItem *AddItem(CFileSetupItem *item, char *name, char *value)
{
	CFileSetupItem *newitem, *lastitem;

	/* can't add field to empty or non-struct item */
	if (!item || item->type != SI_STRUCT) 
		return NULL;

	/* allocate new item */
	newitem = (CFileSetupItem *) malloc(sizeof(CFileSetupItem));

	/* adding name/value pair? */
	if (value)
	{
		/* fill item data */
		newitem->type = SI_FIELD;
		newitem->name = name;
		newitem->data.value = value;
	}
	else
	{
		/* fill item data */
		newitem->type = SI_STRUCT;
		newitem->name = name;
		newitem->data.field = NULL;
	}

	/* find last field of this struct */
	if (!item->data.field) 
	{
		lastitem = NULL;
	}
	else
	{
		lastitem = item->data.field;
		while (lastitem->next)
			lastitem = lastitem->next;
	}

	/* fill item pointers */
	newitem->prev   = lastitem;
	newitem->next   = NULL;
	newitem->parent = item;

	/* link into existing setup structure */
	if (!item->data.field)
		item->data.field = newitem;
	else
		lastitem->next = newitem;

	return newitem;
}

static char *SkipWhite(char *p)
{
	while (*p>'\0' && *p<=' ')
		p++;
	return p;
}

static char *ParseField(char *p, CFileSetupItem *pRootSetupItem)
{
	enum { 
		READFIELDNAME,
		READSUBFIELDNAME,
		FINDEQUALS,
		READVALUE,
		COMMENT,
		ERRORINCOMPLETE,
		ERRORINSTRUCTURE,
		ERRORDUPLICATEFIELD,
		DONE
	} state = READFIELDNAME;
/*
	static const struct CMatching 
	{
		char left, right;
		int skip;
	} matching = {
		{'[',']',0},
		{'\'','\'',1},
	};
*/
	char *pname = NULL, *pvalue, ch;
	int skip;
	CFileSetupItem *pItem = pRootSetupItem, *pNextItem;

	while (state != DONE)
	{
		switch (state)
		{
		case READFIELDNAME:
			p = SkipWhite(p);
			if (*p=='\0') return NULL;
			if (*p=='%') 
			{
				state = COMMENT; 
				break;
			}

		case READSUBFIELDNAME:
			pname = p;
			while ((*p!='.') && (*p!='=')  && (*p!='(')  && (*p!=')')
				&& (*p!=' ') && (*p!='\t') && (*p!='\n') && (*p!='\r') 
				&& (*p!='\0'))
			{
				p++;
			}

			switch (*p)
			{
			case ')':
				if (*(p+1)=='.')
					*p++ = '\0';

			case '(':
			case '.':
				*p++ = '\0';
				pNextItem = SetupFindStruct(pItem, pname);
				if (!pNextItem) pNextItem = AddItem(pItem, pname, NULL);
				if (!pNextItem) 
				{
					state = ERRORINSTRUCTURE;
				}
				else
				{
					pItem = pNextItem;
					state = READSUBFIELDNAME;
				}
				break;

			case '=':
				*p++ = '\0';
				pNextItem = SetupFindField(pItem,pname);
				if (pNextItem) 
				{
					state = ERRORDUPLICATEFIELD;
				}
				else
				{
					state = READVALUE;
				}
				break;

			case ' ':
			case '\t':
			case '\n':
				*p++ = '\0';
				pNextItem = SetupFindField(pItem,pname);
				if (pNextItem) 
				{
					state = ERRORDUPLICATEFIELD;
				}
				else
				{
					state = FINDEQUALS;
				}
				break;

			case '\0':
				state = ERRORINCOMPLETE;
				break;
			}
			break;

		case FINDEQUALS:
			while ((*p!='=') && (*p!='\0'))
				p++;
			if (*p=='=')
			{
				p++;
				state = READVALUE;
			}
			else
			{
				state = ERRORINCOMPLETE;
			}
			break;

		case READVALUE:
			p = SkipWhite(p);

			ch = 0; skip = 0;
			if      (*p=='[' ) { ch = ']';            }
			else if (*p=='\'') { ch = '\''; skip = 1; }

			/*else if (*p=='"')  { ch = '"';  pvalue = ++p; } */

			/* skip over opening bracket */
			p += skip;
			pvalue = p;

			/* find closing bracket? */
			if (ch)
			{
				while ((*p!=ch) && (*p!='\0'))
					p++;

				/* matching closing bracket not found? */
				if (*p!=ch)
				{
					state = ERRORINCOMPLETE;
					break;
				}

				/* skip over closing bracket if necessary */
				p += 1-skip;

				/* zero-terminate string */
				*p++ = '\0';

				/* skip trailing whitespace and semi-colons */
				while (((*p<=' ') && (*p!=0)) || (*p==';'))
					p++;
			}

			/* copy till end-of-line, semi-colon, or end-of-ile */
			else
			{
				while ((*p!='\n') && (*p!='\r') && (*p!=';') && (*p!='\0'))
					p++;

				/* zero-terminate string */
				*p++ = '\0';
			}

			/* add item to setup */
			AddItem(pItem,pname,pvalue);
			state = DONE;
			break;

		case COMMENT:
			while (*p!='\r' && *p!='\n' && *p!=0) 
				p++;
			state = READFIELDNAME;
			break;

		case ERRORINCOMPLETE:
		case ERRORINSTRUCTURE:
		case ERRORDUPLICATEFIELD:
			state = DONE;
			p = NULL;
			break;

        case DONE:
            break;
		}
	}
	return p;
}

int ReadSetup(char *filename, CFileSetup *pSetup)
{
	FILE *fid;
	unsigned long flen;
	char *p;
	size_t fread_bytes = 0;

	/* clear output variable */
	memset(pSetup,0,sizeof(*pSetup));

	fid = fopen(filename,"rb");
	if (!fid) 
		return -1;

	/* determine file length */
	fseek(fid,0,SEEK_END);
	flen = ftell(fid);
	fseek(fid,0,SEEK_SET);

	/* limit file size to 1M */
	if (flen > 1024*1024) 
		return -2;

	/* allocate and clear buffer to hold file plus a little more */
	pSetup->buf    = calloc(flen+4,1);
	pSetup->buflen = flen;

	/* read file into buffer, close file */
	fread_bytes = fread(pSetup->buf,1,flen,fid);
	fclose(fid);

	pSetup->root.type = SI_STRUCT;
	pSetup->root.name = "setup";

	p = pSetup->buf;
	do
	{
		p = ParseField(p,&pSetup->root);
	} while (p);

	return 1;
}

void PrintSetup(CFileSetupItem *item)
{
	if (!item) return;
	if (item->type == SI_FIELD)
	{
		SetupPrintItem(item);
		PrintSetup(item->next);
	}
	else
	{
		PrintSetup(item->data.field);
		PrintSetup(item->next);
	}
}
