/***************************************************************
**
** NanoKit Library Header File
**
** File         :  nkdockview.h
** Module       :  views
** Author       :  SH
** Created      :  2025-04-12 (YYYY-MM-DD)
** License      :  MIT
** Description  :  NanoKit Dock View header file
**
***************************************************************/

#ifndef NKDOCKVIEW_H
#define NKDOCKVIEW_H

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
    bool lastChildFill;   /* Last child fill */
} nkDockView_t;

/***************************************************************
** MARK: FUNCTION DEFS
***************************************************************/

bool nkDockView_Create(nkDockView_t *dockView);

void nkDockView_Destroy(nkDockView_t *dockView);

#endif /* NKDOCKVIEW_H */