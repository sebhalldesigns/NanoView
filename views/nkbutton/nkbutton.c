/***************************************************************
**
** NanoKit Library Source File
**
** File         :  nkbutton.c
** Module       :  view
** Author       :  SH
** Created      :  2025-02-23 (YYYY-MM-DD)
** License      :  MIT
** Description  :  NanoKit Button Control
**
***************************************************************/

/***************************************************************
** MARK: INCLUDES
***************************************************************/

#include "nkbutton.h"

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

static void HoverCallback(nkView_t *view, nkPointerHover_t hover);

static void PointerActionCallback(nkView_t *view, nkPointerAction_t action, nkPointerEvent_t event, float x, float y);

/***************************************************************
** MARK: PUBLIC FUNCTIONS
***************************************************************/

bool nkButton_Create(nkButton_t *button)
{
    if (!nkView_Create(&button->view, "Button"))
    {
        return false;
    }

    /* Set default values */
    button->text = NULL;
    button->onClick = NULL;

    button->view.backgroundColor = NK_COLOR_TRANSPARENT;

    button->view.data = button;
    button->view.dataSize = sizeof(nkButton_t);
    button->view.drawCallback = DrawCallback;
    button->view.measureCallback = MeasureCallback;

    button->view.pointerHoverCallback = HoverCallback;
    button->view.capturePointerHover = true; /* Enable pointer hover capture */
    button->view.pointerActionCallback = PointerActionCallback;
    button->view.capturePointerAction = true; /* Enable pointer action capture */

    button->isHighlighted = false; /* Initial state is not highlighted */
    button->isPressed = false; /* Initial state is not pressed */

    button->background = nkColor_FromHexRGB(0xE0E0E0);
    button->padding = nkThickness_FromConstant(5.0f);
    button->view.margin = nkThickness_FromConstant(5.0f);


    button->view.verticalAlignment = ALIGNMENT_CENTER; /* Center vertically */
    button->view.horizontalAlignment = ALIGNMENT_MIDDLE; /* Center horizontally */

    return true;
}

void nkButton_Destroy(nkButton_t *button)
{

}

/***************************************************************
** MARK: STATIC FUNCTIONS
***************************************************************/

static void DrawCallback(nkView_t *view, nkDrawContext_t *context)
{
    nkButton_t *button = (nkButton_t *)view->data;

    if (button == NULL)
    {
        return;
    }

    if (button->background.a > 0.001f)
    {
        nkColor_t bgColor = button->background;

        if (button->isHighlighted)
        {
            bgColor = nkColor_FromHexRGB(0xF8F8F8);
        }

        nkDraw_SetColorGradient(
            context, 
            nkColor_Lighten(bgColor, 0.5f), 
            bgColor, 
            0.0f, 
            view->frame.x, view->frame.y, 
            view->frame.width, view->frame.height
        );

        nkDraw_RoundedRect(
            context, 
            view->frame.x, view->frame.y, 
            view->frame.width, view->frame.height, 
            5.0f
        );

        nkDraw_SetStrokeWidth(context, 1.0f);

        nkDraw_SetStrokeColorGradient(
            context, 
            nkColor_Lighten(bgColor, 0.75f), 
            nkColor_Darken(bgColor, 0.125f), 
            0.0f, 
            view->frame.x, view->frame.y, 
            view->frame.width, view->frame.height
        );

        nkDraw_RoundedRectPath(
            context, 
            view->frame.x, view->frame.y, 
            view->frame.width, view->frame.height, 
            5.0f
        );

    }

    if (button->text)
    {
        nkDraw_SetColor(context, NK_COLOR_BLACK);
        nkDraw_Text(context, button->font, button->text, view->frame.x + button->padding.left, view->frame.y + 12.0f + button->padding.top);
    }

}

static void MeasureCallback(nkView_t *view, nkDrawContext_t *context)
{
    nkButton_t *button = (nkButton_t *)view->data;

    if (button == NULL)
    {
        return;
    }

    nkRect_t textFrame = nkDraw_MeasureText(context, button->font, button->text ? button->text : "");

    view->sizeRequest.width = textFrame.width + button->padding.left + button->padding.right;
    view->sizeRequest.height = textFrame.height + button->padding.top + button->padding.bottom;

}

static void HoverCallback(nkView_t *view, nkPointerHover_t hover)
{
    nkButton_t *button = (nkButton_t *)view->data;

    if (button == NULL)
    {
        return;
    }

    if (hover == HOVER_BEGIN)
    {
        button->isHighlighted = true;
    }
    else if (hover == HOVER_END)
    {
        button->isHighlighted = false;
    }
}

static void PointerActionCallback(nkView_t *view, nkPointerAction_t action, nkPointerEvent_t event, float x, float y)
{
    nkButton_t *button = (nkButton_t *)view->data;

    if (button == NULL)
    {
        return;
    }

    if (event == POINTER_EVENT_BEGIN)
    {
        button->isPressed = true;
    }
    else if (event == POINTER_EVENT_END)
    {
        button->isPressed = false;

        if (button->onClick)
        {
            button->onClick(button);
        }
    }
    else if (event == POINTER_EVENT_CANCEL)
    {
        button->isPressed = false;
    }
}