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
static void MeasureCallback(nkView_t *view, nkDrawContext_t *context);

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
    label->view.measureCallback = MeasureCallback;

    label->view.data = label;
    label->view.dataSize = sizeof(nkLabel_t);

    label->foreground = NK_COLOR_BLACK; /* Default foreground color */
    label->padding = nkThickness_FromConstant(0.0f);
    label->view.margin = nkThickness_FromConstant(0.0f);


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
        nkDraw_SetColor(context, label->foreground);
        nkDraw_Text(context, label->font, label->text, view->frame.x + label->padding.left, view->frame.y + 12.0f + label->padding.top);
    }

}

static void MeasureCallback(nkView_t *view, nkDrawContext_t *context)
{
    nkLabel_t *label = (nkLabel_t *)view->data;

    if (label == NULL)
    {
        return;
    }

    nkRect_t textFrame = nkDraw_MeasureText(context, label->font, label->text ? label->text : "");

    view->sizeRequest.width = textFrame.width + label->padding.left + label->padding.right;
    view->sizeRequest.height = textFrame.height + label->padding.top + label->padding.bottom;

}