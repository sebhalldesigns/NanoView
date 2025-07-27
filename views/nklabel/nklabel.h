/***************************************************************
**
** NanoKit Library Header File
**
** File         :  nklabel.h
** Module       :  views
** Author       :  SH
** Created      :  2025-07-27 (YYYY-MM-DD)
** License      :  MIT
** Description  :  NanoKit Label Control header file
**
***************************************************************/

#ifndef NKLABEL_H
#define NKLABEL_H

/***************************************************************
** MARK: INCLUDES
***************************************************************/

#include <nanoview.h>

/***************************************************************
** MARK: CONSTANTS & MACROS
***************************************************************/

/***************************************************************
** MARK: TYPEDEFS
***************************************************************/

typedef struct 
{
    nkView_t view;          /* view */

    nkThickness_t padding;  /* padding around the button contents */

    const char *text;       /* button text */
    nkFont_t *font;         /* button font */
    nkColor_t foreground; 

    nkColor_t background;   /* button background color */
    
} nkLabel_t;

/***************************************************************
** MARK: FUNCTION DEFS
***************************************************************/

bool nkLabel_Create(nkLabel_t *label);

void nkLabel_Destroy(nkLabel_t *label);

#endif /* NKLABEL_H */