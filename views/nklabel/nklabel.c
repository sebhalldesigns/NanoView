/***************************************************************
**
** NanoKit Library Source File
**
** File         :  nklabel.c
** Module       :  view
** Author       :  SH
** Created      :  2025-02-23 (YYYY-MM-DD)
** License      :  MIT
** Description  :  NanoKit Label Control
**
***************************************************************/

/***************************************************************
** MARK: INCLUDES
***************************************************************/

#include "nklabel.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/***************************************************************
** MARK: CONSTANTS & MACROS
***************************************************************/

/***************************************************************
** MARK: TYPEDEFS
***************************************************************/

/***************************************************************
** MARK: STATIC VARIABLES
***************************************************************/

/***************************************************************
** MARK: STATIC FUNCTION DEFS
***************************************************************/

static void DrawCallback(nkView_t *view, nkDrawContext_t *context);

/***************************************************************
** MARK: PUBLIC FUNCTIONS
***************************************************************/

bool nkLabel_Create(nkLabel_t *label)
{
    if (!nkView_Create(&label->view, "Label"))
    {
        return false;
    }

    /* Set default values */
    label->text = NULL;

    label->view.drawCallback = DrawCallback;

    label->view.data = label;
    label->view.dataSize = sizeof(nkLabel_t);

    return true;
}

void nkLabel_Destroy(nkLabel_t *label)
{

}

/***************************************************************
** MARK: STATIC FUNCTIONS
***************************************************************/

static void DrawCallback(nkView_t *view, nkDrawContext_t *context)
{
    nkLabel_t *label = (nkLabel_t *)view->data;

    if (label == NULL)
    {
        return;
    }

    if (label->background.a > 0.001f)
    {
        nkColor_t bgColor = label->background;

        nkDraw_SetColor(context, bgColor);
        nkDraw_Rect(context, view->frame.x, view->frame.y, view->frame.width, view->frame.height);
    }

    if (label->text)
    {
        nkDraw_SetColor(context, NK_COLOR_BLACK);
        nkDraw_Text(context, label->font, label->text, view->frame.x + label->padding.left, view->frame.y + 12.0f + label->padding.top);
    }

}
