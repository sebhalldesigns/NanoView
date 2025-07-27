/***************************************************************
**
** NanoKit Library Source File
**
** File         :  nkdockview.c
** Module       :  views
** Author       :  SH
** Created      :  2025-02-23 (YYYY-MM-DD)
** License      :  MIT
** Description  :  NanoKit Dock View
**
***************************************************************/

/***************************************************************
** MARK: INCLUDES
***************************************************************/

#include "nkdockview.h"

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

static void MeasureCallback(nkView_t *view);
static void ArrangeCallback(nkView_t *view);

/***************************************************************
** MARK: PUBLIC FUNCTIONS
***************************************************************/

bool nkDockView_Create(nkDockView_t *dockView)
{
    if (!nkView_Create(&dockView->view, "DockPanel"))
    {
        return false;
    }

    dockView->view.measureCallback = MeasureCallback;
    dockView->view.arrangeCallback = ArrangeCallback;

    /* Set default values */
    dockView->lastChildFill = true;

    dockView->view.data = dockView;
    dockView->view.dataSize = sizeof(nkDockView_t);

    return true;
}

void nkDockPanel_Destroy(nkDockView_t *dockView)
{

}


/***************************************************************
** MARK: STATIC FUNCTIONS
***************************************************************/

static void MeasureCallback(nkView_t *view)
{
    //printf("DOCKPANEL MEASURE\n");

    nkSize_t total = {0, 0};
    nkView_t *child = view->child;

    while (child)
    {

        nkSize_t marginRequest = child->sizeRequest;
        marginRequest.width += child->margin.left + child->margin.right;
        marginRequest.height += child->margin.top + child->margin.bottom;

        if (child->dockPosition == DOCK_POSITION_LEFT || child->dockPosition == DOCK_POSITION_RIGHT)
        {
            total.width += marginRequest.width;
            
            if (marginRequest.height > total.height)
            {
                total.height = marginRequest.height;
            }
                
        }
        else if (child->dockPosition == DOCK_POSITION_TOP || child->dockPosition == DOCK_POSITION_BOTTOM)
        {
            total.height += marginRequest.height;
            
            if (marginRequest.width > total.width)
            {
                total.width = marginRequest.width;
            }
        }

        child = child->sibling;
    }

    view->sizeRequest = total;
}

static void ArrangeCallback(nkView_t *view)
{

    //printf("DOCKPANEL ARRANGE\n");

    nkRect_t client = view->frame; /* available space inside this view */
    nkView_t *child = view->child;

    while (child)
    {

        if (!child->sibling)
        {
            child->frame = client;
            return;
        }

        nkRect_t childRect = client;

        nkSize_t marginRequest = child->sizeRequest;
        marginRequest.width += child->margin.left + child->margin.right;
        marginRequest.height += child->margin.top + child->margin.bottom;

        switch (child->dockPosition)
        {
            case DOCK_POSITION_LEFT:
            {
                childRect.width = marginRequest.width;
                childRect.height = client.height;
                
                if (childRect.width > client.width)
                {
                    childRect.width = client.width;
                }

                client.x += childRect.width;
                client.width -= childRect.width;
            } break;

            case DOCK_POSITION_RIGHT:
            {
                childRect.x = client.x + client.width - marginRequest.width;
                childRect.width = marginRequest.width;
                
                if (childRect.width > client.width)
                {
                    childRect.width = client.width;
                }
                
                childRect.height = client.height;
                client.width -= childRect.width;
            } break;

            case DOCK_POSITION_TOP:
            {
                childRect.height = marginRequest.height;
                childRect.width = client.width;
                
                if (childRect.height > client.height)
                {
                    childRect.height = client.height;
                }

                client.y += childRect.height;
                client.height -= childRect.height;
            } break;

            case DOCK_POSITION_BOTTOM:
            {

                childRect.y = client.y + client.height - marginRequest.height;
                childRect.height = marginRequest.height;
                
                if (childRect.height > client.height)
                {
                    childRect.height = client.height;
                }

                childRect.width = client.width;
                client.height -= childRect.height;
            } break;

            default:
            {
                /* shouldn't get to here */
            } break;
        }

        nkView_PlaceView(child, childRect);

        child = child->sibling;
    }
}