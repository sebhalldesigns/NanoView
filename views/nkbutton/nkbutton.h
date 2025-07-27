/***************************************************************
**
** NanoKit Library Header File
**
** File         :  nkbutton.h
** Module       :  views
** Author       :  SH
** Created      :  2025-07-27 (YYYY-MM-DD)
** License      :  MIT
** Description  :  NanoKit Button Control header file
**
***************************************************************/

#ifndef NKBUTTON_H
#define NKBUTTON_H

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

struct nkButton_t; /* forward declaration */

typedef void (*nkButtonCallback_t)(struct nkButton_t *button);

typedef struct 
{
    nkView_t view;          /* view */

    nkThickness_t padding;  /* padding around the button contents */

    const char *text;       /* button text */
    nkFont_t *font;         /* button font */
    nkColor_t foreground; 

    float cornerRadius;     /* corner radius for rounded buttons */
    nkColor_t background;   /* button background color */

    nkButtonCallback_t onClick; /* callback for button click */

    /* state */
    bool isHighlighted; /* true if the button is highlighted */
    bool isPressed;     /* true if the button is pressed */
    
} nkButton_t;

/***************************************************************
** MARK: FUNCTION DEFS
***************************************************************/

bool nkButton_Create(nkButton_t *button);

void nkButton_Destroy(nkButton_t *button);

#endif /* NKBUTTON_H */