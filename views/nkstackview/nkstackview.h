/***************************************************************
**
** NanoKit Library Header File
**
** File         :  nkstackview.h
** Module       :  views
** Author       :  SH
** Created      :  2025-08-17 (YYYY-MM-DD)
** License      :  MIT
** Description  :  NanoKit Stack View header file
**
***************************************************************/

#ifndef NKSTACKVIEW_H
#define NKSTACKVIEW_H

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

typedef enum
{
    STACK_ORIENTATION_HORIZONTAL,
    STACK_ORIENTATION_VERTICAL
} nkStackOrientation_t;

typedef struct 
{
    nkView_t view;          /* view */
    nkStackOrientation_t orientation;
} nkStackView_t;

/***************************************************************
** MARK: FUNCTION DEFS
***************************************************************/

bool nkStackView_Create(nkStackView_t *stackView);

void nkStackView_Destroy(nkStackView_t *stackView);

#endif /* NKSTACKVIEW_H */