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

    button->view.data = button;
    button->view.dataSize = sizeof(nkButton_t);
    button->view.drawCallback = DrawCallback;

    button->view.pointerHoverCallback = HoverCallback;
    button->view.capturePointerHover = true; /* Enable pointer hover capture */
    button->view.pointerActionCallback = PointerActionCallback;
    button->view.capturePointerAction = true; /* Enable pointer action capture */

    button->isHighlighted = false; /* Initial state is not highlighted */
    button->isPressed = false; /* Initial state is not pressed */

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

        if (button->isPressed)
        {
            bgColor.r *= 1.4f;
            bgColor.g *= 1.4f;
            bgColor.b *= 1.4f;
        }
        else if (button->isHighlighted)
        {
            bgColor.r *= 1.3f;
            bgColor.g *= 1.3f;
            bgColor.b *= 1.3f;
        }

        nkDraw_SetColor(context, bgColor);
        nkDraw_Rect(context, view->frame.x, view->frame.y, view->frame.width, view->frame.height);
    }

    if (button->text)
    {
        nkDraw_SetColor(context, NK_COLOR_BLACK);
        nkDraw_Text(context, button->font, button->text, view->frame.x + button->padding.left, view->frame.y + 12.0f + button->padding.top);
    }

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