/***************************************************************
**
** NanoKit Library Source File
**
** File         :  nkscrollview.c
** Module       :  views
** Author       :  SH
** Created      :  2025-08-17 (YYYY-MM-DD)
** License      :  MIT
** Description  :  NanoKit Scroll View
**
***************************************************************/

/***************************************************************
** MARK: INCLUDES
***************************************************************/

#include "nkscrollview.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

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

static void ArrangeCallback(nkView_t *view, nkDrawContext_t *context);

static void ScrollCallback(nkView_t *view, float delta);

static void HoverCallback(nkView_t *view, nkPointerHover_t hover);

static void PointerMovementCallback(nkView_t *view, float x, float y);

static void PointerActionCallback(nkView_t *view, nkPointerAction_t action, nkPointerEvent_t event, float x, float y);


/***************************************************************
** MARK: PUBLIC FUNCTIONS
***************************************************************/

bool nkScrollView_Create(nkScrollView_t *scrollView)
{
    if (!nkView_Create(&scrollView->view, "ScrollView"))
    {
        return false;
    }

    scrollView->view.measureCallback = NULL; /* No measure callback for scrollviewer */
    scrollView->view.arrangeCallback = ArrangeCallback;
    scrollView->view.drawCallback = DrawCallback;

    scrollView->view.pointerHoverCallback = HoverCallback;
    scrollView->view.capturePointerHover = true; /* Enable pointer hover capture */
    scrollView->view.pointerActionCallback = PointerActionCallback;
    scrollView->view.capturePointerAction = true; /* Enable pointer action capture */

    scrollView->view.pointerMovementCallback = PointerMovementCallback;
    scrollView->view.capturePointerMovement = true; /* Enable pointer movement capture */

    scrollView->view.scrollCallback = ScrollCallback; /* No scroll callback by default */
    scrollView->view.captureScroll = true; /* Enable scroll capture */


    scrollView->view.data = scrollView;
    scrollView->view.dataSize = sizeof(nkScrollView_t);

    /* default state */
    scrollView->verticalScrollRatio = 1.0f;
    scrollView->verticalScrollOffset = 0.0f;

    scrollView->horizontalScrollRatio = 1.0f;
    scrollView->horizontalScrollOffset = 0.0f;

    scrollView->verticalScrollBar = (nkRect_t){0, 0, 0, 0};
    scrollView->horizontalScrollBar = (nkRect_t){0, 0, 0, 0};

    scrollView->isVerticalScrollHighlighted = false;
    scrollView->isVerticalScrollPressed = false;

    scrollView->isHorizontalScrollHighlighted = false;
    scrollView->isHorizontalScrollPressed = false;

    scrollView->dragOrigin = (nkPoint_t){0, 0};

    scrollView->needsLayout = false; /* Initially needs layout */
    scrollView->view.clipToBounds = true; /* Clip to bounds by default */

    return true;
}

void nkScrollView_Destroy(nkScrollView_t *scrollView)
{

}


/***************************************************************
** MARK: STATIC FUNCTIONS
***************************************************************/

static void ArrangeCallback(nkView_t *view, nkDrawContext_t *context)
{

    nkScrollView_t *scrollView = (nkScrollView_t *)view->data;

    if (scrollView == NULL || view->dataSize != sizeof(nkScrollView_t))
    {
        return;
    }


    nkRect_t client = view->frame; /* available space inside this view */
    nkView_t *child = view->child;

    if (child)
    {
        nkRect_t childRect = client;

        nkSize_t marginRequest = child->sizeRequest;
        marginRequest.width += child->margin.left + child->margin.right;
        marginRequest.height += child->margin.top + child->margin.bottom;

        childRect.x = client.x - scrollView->horizontalScrollOffset * (marginRequest.width);
        childRect.y = client.y - scrollView->verticalScrollOffset * (marginRequest.height);
        childRect.width = marginRequest.width;
        childRect.height = marginRequest.height;

        nkView_PlaceView(child, childRect);

        scrollView->verticalScrollRatio = view->child->frame.height / view->frame.height;
        scrollView->horizontalScrollRatio = view->child->frame.width / view->frame.width;

        /* clip scroll offset to max value */
        scrollView->verticalScrollOffset = fmaxf(0.0f, fminf(1.0f - (1.0f / scrollView->verticalScrollRatio), scrollView->verticalScrollOffset));
        scrollView->horizontalScrollOffset = fmaxf(0.0f, fminf(1.0f - (1.0f / scrollView->horizontalScrollRatio), scrollView->horizontalScrollOffset));

        if (scrollView->verticalScrollRatio > 1.0f)
        {
            float barHeight = view->frame.height * (1.0f / scrollView->verticalScrollRatio);
            float maxOffset = 1.0f - (1.0f / scrollView->verticalScrollRatio);

            scrollView->verticalScrollBar = (nkRect_t) {
                .x = view->frame.x + view->frame.width - 10.0f,
                .y = view->frame.y + ((scrollView->verticalScrollOffset / maxOffset) * (view->frame.height - barHeight)),
                .width = 10.0f,
                .height = barHeight
            };

        }

        if (scrollView->horizontalScrollRatio > 1.0f)
        {

            float barWidth = view->frame.width * (1.0f / scrollView->horizontalScrollRatio);
            float maxOffset = 1.0f - (1.0f / scrollView->horizontalScrollRatio);

            scrollView->horizontalScrollBar = (nkRect_t) {
                .x = view->frame.x + ((scrollView->horizontalScrollOffset / maxOffset) * (view->frame.width - barWidth)),
                .y = view->frame.y + view->frame.height - 10.0f,
                .width = barWidth,
                .height = 10.0f
            };
        }
    }
}

static void DrawCallback(nkView_t *view, nkDrawContext_t *context)
{
    
    nkScrollView_t *scrollView = (nkScrollView_t *)view->data;

    if (scrollView == NULL || view->dataSize != sizeof(nkScrollView_t))
    {
        return;
    }

    if (scrollView->needsLayout)
    {
        nkView_LayoutSubtree(view, context);
        scrollView->needsLayout = false; /* Reset layout flag */
    }

    if (!view->child)
    {
        return;
    }

    
    if (scrollView->verticalScrollRatio > 1.0f)
    {
        /* draw vertical scrollbar */
         
        if (scrollView->isVerticalScrollHighlighted)
        {
            nkDraw_SetColor(context, nkColor_Lighten(NK_COLOR_DARK_GRAY, 0.25f));
        } 
        else
        {
            nkDraw_SetColor(context, NK_COLOR_DARK_GRAY);
        }

        nkDraw_Rect(
            context, 
            scrollView->verticalScrollBar.x, scrollView->verticalScrollBar.y, 
            scrollView->verticalScrollBar.width, scrollView->verticalScrollBar.height
        );
    }

    if (scrollView->horizontalScrollRatio > 1.0f)
    {
        /* draw horizontal scrollbar */

        if (scrollView->isHorizontalScrollHighlighted)
        {
            nkDraw_SetColor(context, nkColor_Lighten(NK_COLOR_DARK_GRAY, 0.25f));
        } 
        else
        {
            nkDraw_SetColor(context, NK_COLOR_DARK_GRAY);
        }

        nkDraw_Rect(
            context, 
            scrollView->horizontalScrollBar.x, scrollView->horizontalScrollBar.y, 
            scrollView->horizontalScrollBar.width, scrollView->horizontalScrollBar.height
        );
    }
}

static void ScrollCallback(nkView_t *view, float delta)
{
    //printf("ScrollView scrolled by: %f\n", delta);
    
    nkScrollView_t *scrollView = (nkScrollView_t *)view->data;

    if (scrollView == NULL || view->dataSize != sizeof(nkScrollView_t))
    {
        return;
    }
    
    if (scrollView->verticalScrollRatio > 1.0f)
    {
        scrollView->verticalScrollOffset -= (delta * 30.0f) / view->child->frame.height;

        scrollView->verticalScrollOffset = fmaxf(0.0f, fminf(1.0f - (1.0f / scrollView->verticalScrollRatio), scrollView->verticalScrollOffset));
        
        scrollView->needsLayout = true; /* Mark layout as needing update */
    }
    else if (scrollView->horizontalScrollRatio > 1.0f)
    {
        scrollView->horizontalScrollOffset -= (delta * 30.0f) / view->child->frame.width;

        scrollView->horizontalScrollOffset = fmaxf(0.0f, fminf(1.0f - (1.0f / scrollView->horizontalScrollRatio), scrollView->horizontalScrollOffset));

        scrollView->needsLayout = true; /* Mark layout as needing update */
    }
}

static void HoverCallback(nkView_t *view, nkPointerHover_t hover)
{
    nkScrollView_t *scrollView = (nkScrollView_t *)view->data;

    if (scrollView == NULL || view->dataSize != sizeof(nkScrollView_t))
    {
        return;
    }

    if (hover == HOVER_END)
    {
        scrollView->isVerticalScrollHighlighted = false;
        scrollView->isHorizontalScrollHighlighted = false;
    }
}

static void PointerMovementCallback(nkView_t *view, float x, float y)
{
    nkScrollView_t *scrollView = (nkScrollView_t *)view->data;

    if (scrollView == NULL || view->dataSize != sizeof(nkScrollView_t))
    {
        return;
    }

    if (nkRect_ContainsPoint(scrollView->verticalScrollBar, (nkPoint_t){x, y}))
    {
        scrollView->isVerticalScrollHighlighted = true;
    }
    else if (nkRect_ContainsPoint(scrollView->horizontalScrollBar, (nkPoint_t){x, y}))
    {
        scrollView->isHorizontalScrollHighlighted = true;
    }
    else
    {
        scrollView->isVerticalScrollHighlighted = false;
        scrollView->isHorizontalScrollHighlighted = false;
    }
}   

static void PointerActionCallback(nkView_t *view, nkPointerAction_t action, nkPointerEvent_t event, float x, float y)
{
    nkScrollView_t *scrollView = (nkScrollView_t *)view->data;

    if (scrollView == NULL || view->dataSize != sizeof(nkScrollView_t) || view->child == NULL)
    {
        return;
    }

    switch (event)
    {
        case POINTER_EVENT_BEGIN:
        {
            if (nkRect_ContainsPoint(scrollView->verticalScrollBar, (nkPoint_t){x, y}))
            {
                scrollView->isVerticalScrollPressed = true;
                scrollView->dragOrigin = (nkPoint_t){x, y};
                scrollView->dragStartOffset = (nkPoint_t){scrollView->horizontalScrollOffset, scrollView->verticalScrollOffset};
            }
            else if (nkRect_ContainsPoint(scrollView->horizontalScrollBar, (nkPoint_t){x, y}))
            {
                scrollView->isHorizontalScrollPressed = true;
                scrollView->dragOrigin = (nkPoint_t){x, y};
                scrollView->dragStartOffset = (nkPoint_t){scrollView->horizontalScrollOffset, scrollView->verticalScrollOffset};
            }

        } break;

        case POINTER_EVENT_END:
        {
            scrollView->isVerticalScrollPressed = false;
            scrollView->isHorizontalScrollPressed = false;
        } break;

        case POINTER_EVENT_DRAG:
        {
            if (scrollView->isVerticalScrollPressed)
            {
                float deltaY = y - scrollView->dragOrigin.y;
                scrollView->verticalScrollOffset = scrollView->dragStartOffset.y + deltaY / view->child->frame.height * scrollView->verticalScrollRatio;

                scrollView->verticalScrollOffset = fmaxf(0.0f, fminf(1.0f - (1.0f / scrollView->verticalScrollRatio), scrollView->verticalScrollOffset));

                scrollView->needsLayout = true; /* Mark layout as needing update */
            }

            if (scrollView->isHorizontalScrollPressed)
            {
                float deltaX = x - scrollView->dragOrigin.x;
                scrollView->horizontalScrollOffset = scrollView->dragStartOffset.x + deltaX / view->child->frame.width * scrollView->horizontalScrollRatio;

                scrollView->horizontalScrollOffset = fmaxf(0.0f, fminf(1.0f - (1.0f / scrollView->horizontalScrollRatio), scrollView->horizontalScrollOffset));
                
                scrollView->needsLayout = true; /* Mark layout as needing update */
            }
        } break;

        default:
        {

        } break;
    }

}