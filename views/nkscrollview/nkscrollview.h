/***************************************************************
**
** NanoKit Library Header File
**
** File         :  nkscrollview.h
** Module       :  views
** Author       :  SH
** Created      :  2025-08-17 (YYYY-MM-DD)
** License      :  MIT
** Description  :  NanoKit Scroll View header file
**
***************************************************************/

#ifndef NKSCROLLVIEW_H
#define NKSCROLLVIEW_H

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

    bool needsLayout; /* true if layout needs to be recalculated */

    float verticalScrollRatio; 
    float verticalScrollLimit;
    float verticalScrollOffset;

    float horizontalScrollRatio; 
    float horizontalScrollOffset;

    nkRect_t verticalScrollBar;
    nkRect_t horizontalScrollBar;

    bool isVerticalScrollHighlighted;
    bool isVerticalScrollPressed; 

    bool isHorizontalScrollHighlighted;
    bool isHorizontalScrollPressed;

    nkPoint_t dragOrigin;
    nkPoint_t dragStartOffset;
} nkScrollView_t;

/***************************************************************
** MARK: FUNCTION DEFS
***************************************************************/

bool nkScrollView_Create(nkScrollView_t *scrollView);

void nkScrollView_Destroy(nkScrollView_t *scrollView);

#endif /* NKSCROLLVIEW_H */