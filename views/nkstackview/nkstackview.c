/***************************************************************
**
** NanoKit Library Source File
**
** File         :  nkstackview.c
** Module       :  views
** Author       :  SH
** Created      :  2025-08-17 (YYYY-MM-DD)
** License      :  MIT
** Description  :  NanoKit Stack View
**
***************************************************************/

/***************************************************************
** MARK: INCLUDES
***************************************************************/

#include "nkstackview.h"

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

static void MeasureCallback(nkView_t *view, nkDrawContext_t *context);
static void ArrangeCallback(nkView_t *view, nkDrawContext_t *context);

/***************************************************************
** MARK: PUBLIC FUNCTIONS
***************************************************************/

bool nkStackView_Create(nkStackView_t *stackView)
{
    if (!nkView_Create(&stackView->view, "StackView"))
    {
        return false;
    }

    stackView->view.measureCallback = MeasureCallback;
    stackView->view.arrangeCallback = ArrangeCallback;

    /* Set default values */
    stackView->orientation = STACK_ORIENTATION_HORIZONTAL;

    stackView->view.data = stackView;
    stackView->view.dataSize = sizeof(nkStackView_t);

    return true;
}

void nkStackView_Destroy(nkStackView_t *stackView)
{

}


/***************************************************************
** MARK: STATIC FUNCTIONS
***************************************************************/

static void MeasureCallback(nkView_t *view, nkDrawContext_t *context)
{

    nkStackView_t *stackView = (nkStackView_t *)view->data;

    if (!stackView || view->dataSize != sizeof(nkStackView_t))
    {
        return;
    }

    nkSize_t total = {0, 0};
    nkView_t *child = view->child;

    while (child)
    {

        nkSize_t marginRequest = child->sizeRequest;
        marginRequest.width += child->margin.left + child->margin.right;
        marginRequest.height += child->margin.top + child->margin.bottom;

        switch (stackView->orientation)
        {
            case STACK_ORIENTATION_VERTICAL:
            {
                total.width = fmaxf(total.width, marginRequest.width);
                total.height += marginRequest.height;
            } break;

            default:
            {   
                total.width += marginRequest.width;
                total.height = fmaxf(total.height, marginRequest.height);
            } break;
        }

        child = child->sibling;
    }

    view->sizeRequest = total;
}

static void ArrangeCallback(nkView_t *view, nkDrawContext_t *context)
{

    nkStackView_t *stackView = (nkStackView_t *)view->data;

    if (!stackView || view->dataSize != sizeof(nkStackView_t))
    {
        return;
    }

    nkRect_t client = view->frame; /* available space inside this view */
    nkView_t *child = view->child;

    while (child)
    {

        nkRect_t childRect = client;

        nkSize_t marginRequest = child->sizeRequest;
        marginRequest.width += child->margin.left + child->margin.right;
        marginRequest.height += child->margin.top + child->margin.bottom;

        switch (stackView->orientation)
        {
            case STACK_ORIENTATION_VERTICAL:
            {
                childRect.width = client.width;
                childRect.height = marginRequest.height;

                if (childRect.height > client.height)
                {
                    childRect.height = client.height;
                }

                client.y += childRect.height;
                client.height -= childRect.height;
            } break;

            default:
            {
                childRect.height = client.height;
                childRect.width = marginRequest.width;

                if (childRect.width > client.width)
                {
                    childRect.width = client.width;
                }

                client.x += childRect.width;
                client.width -= childRect.width;
            } break;
        }

        nkView_PlaceView(child, childRect);

        child = child->sibling;
    }
}