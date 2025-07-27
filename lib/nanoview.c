/***************************************************************
**
** NanoKit Library Source File
**
** File         :  view.c
** Module       :  view
** Author       :  SH
** Created      :  2025-02-23 (YYYY-MM-DD)
** License      :  MIT
** Description  :  NanoKit View API
**
***************************************************************/

/***************************************************************
** MARK: INCLUDES
***************************************************************/

#include <nanoview.h>
#include <nanodraw.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

/***************************************************************
** MARK: PUBLIC FUNCTIONS
***************************************************************/


bool nkView_Create(nkView_t *view, const char *name)
{
    view->name = name;

    view->frame = (nkRect_t){0, 0, 0, 0};
    view->sizeRequest = (nkSize_t){0, 0};
    view->margin = (nkThickness_t){0, 0, 0, 0};

    view->parent = NULL;
    view->sibling = NULL;
    view->prevSibling = NULL;
    view->child = NULL;

    view->horizontalAlignment = ALIGNMENT_STRETCH;
    view->verticalAlignment = ALIGNMENT_FILL;

    view->dockPosition = DOCK_POSITION_TOP;
    view->gridLocation = (nkGridLocation_t){0, 0, 1, 1};
    view->canvasRect = (nkRect_t){0, 0, 0, 0};

    view->measureCallback = NULL;
    view->arrangeCallback = NULL;
    view->drawCallback = NULL;
    view->destroyCallback = NULL;

    view->backgroundColor = NK_COLOR_TRANSPARENT;

    view->data = NULL;
    view->dataSize = 0; /* size of the data in bytes */

    return view;
}

void nkView_Destroy(nkView_t *view)
{
    if (view == NULL)
    {
        return;
    }
    
    return;
}

void nkView_LayoutTree(nkView_t *root, nkSize_t size)
{
    if (root == NULL)
    {
        return;
    }

    nkView_t *view = root;

    root->frame = (nkRect_t){0, 0, size.width, size.height};


    /* MEASURE PASS */

    view = nkView_DeepestViewInTree(root);
    
    /* measure views in a bottom-up traversal */

    while (view)
    {
        if (view->measureCallback)
        {
            view->measureCallback(view);
        }

        view = nkView_PreviousViewInTree(view);
    }

    /* ARRANGE PASS */

    view = root;

    /* clamp root to size */

    if (root->sizeRequest.width > root->frame.width)
    {
        root->frame.width = root->sizeRequest.width;
    }
    
    if (root->sizeRequest.height > root->frame.height)
    {
        root->frame.height = root->sizeRequest.height;
    }

    /* arrange views in top-down */
    while (view)
    {

        if (view->arrangeCallback)
        {
            view->arrangeCallback(view);
        }

        view = nkView_NextViewInTree(view);
    }

}

void nkView_RenderTree(nkView_t *root, nkDrawContext_t *drawContext)
{
    if (root == NULL || drawContext == NULL)
    {
        return;
    }

    nkView_t *view = root;

    /* render views in a top-down traversal (this is actually bottom up in visual tree 
       as child views appear on top) */

    while (view)
    {

        if (view->backgroundColor.a > 0.001f)
        {
            nkDraw_SetColor(drawContext, view->backgroundColor);
            nkDraw_Rect(drawContext, view->frame.x, view->frame.y, view->frame.width, view->frame.height);
        }

        if (view->drawCallback)
        {
            view->drawCallback(view, drawContext);
        }

        view = nkView_NextViewInTree(view);
    }

}

void nkView_ProcessPointerMovement(nkView_t *root, float x, float y, nkView_t **hotView, nkView_t *activeView, nkPointerAction_t activeAction)
{
    if (root == NULL || hotView == NULL)
    {
        return;
    }

    nkView_t *newHotView = nkView_HitTest(root, x, y);

    /* check for change */
    if (*hotView != newHotView)
    {
        /* Pointer moved to a different view, process the pointer movement */
        if (*hotView != NULL && (*hotView)->capturePointerHover && (*hotView)->pointerHoverCallback)
        {
            (*hotView)->pointerHoverCallback(*hotView, HOVER_END);
        }

        if (newHotView != NULL && newHotView->capturePointerHover && newHotView->pointerHoverCallback)
        {
            newHotView->pointerHoverCallback(newHotView, HOVER_BEGIN);
        }

        *hotView = newHotView; /* Update the hot view */
    }
    else if (*hotView != NULL && newHotView->capturePointerMovement && (*hotView)->pointerMovementCallback)
    {
        (*hotView)->pointerMovementCallback(*hotView, x, y);
    }

    /* If there is an active view, process the pointer movement in it */
    if (activeView != NULL && activeView->capturePointerAction && activeView->pointerActionCallback)
    {
        activeView->pointerActionCallback(activeView, activeAction, POINTER_EVENT_DRAG, x, y); 
    }
}

void nkView_ProcessPointerAction(nkView_t *root, nkPointerAction_t action, nkPointerEvent_t event, float x, float y, nkView_t *hotView, nkView_t **activeView, nkPointerAction_t *activeAction)
{

    if (root == NULL || activeView == NULL || activeAction == NULL)
    {

        return;
    }

    switch (event)
    {
        case POINTER_EVENT_BEGIN:
        {
            /* check if there is an active view */
            if (*activeView != hotView)
            {
                /* if there is an active view, end the action */
                if (*activeView != NULL && (*activeView)->capturePointerAction && (*activeView)->pointerActionCallback)
                {
                    (*activeView)->pointerActionCallback(*activeView, *activeAction, POINTER_EVENT_END, x, y);
                }

            }
            
            /* set the new active view */
            *activeView = hotView;
            *activeAction = action;

            if (hotView && hotView->capturePointerAction && hotView->pointerActionCallback)
            {
                hotView->pointerActionCallback(hotView, action, POINTER_EVENT_BEGIN, x, y);
            }

        } break;

        case POINTER_EVENT_END:
        {
            if (*activeView != NULL)
            {
                /* call the action end callback */
                if ((*activeView)->capturePointerAction && (*activeView)->pointerActionCallback)
                {
                    (*activeView)->pointerActionCallback(*activeView, *activeAction, POINTER_EVENT_END, x, y);
                }

                /* reset active view and action */
                *activeView = NULL;
                *activeAction = NK_POINTER_ACTION_PRIMARY; /* reset to default primary action */
            }
        } break;

        case POINTER_EVENT_CANCEL:
        {
            /* cancel the action if there is an active view */
            if (*activeView != NULL)
            {
                if ((*activeView)->capturePointerAction && (*activeView)->pointerActionCallback)
                {
                    (*activeView)->pointerActionCallback(*activeView, *activeAction, POINTER_EVENT_CANCEL, x, y);
                }

                /* reset active view and action */
                *activeView = NULL;
                *activeAction = NK_POINTER_ACTION_PRIMARY; /* reset to default primary action */
            }
        } break;

        case POINTER_EVENT_DOUBLE:
        {
            /* double click, TODO */
        } break;
    }
}

/* tree structure functions */
void nkView_AddChildView(nkView_t *parent, nkView_t *child)
{
    if (parent == NULL || child == NULL)
    {
        return;
    }

    child->parent = parent;
    child->prevSibling = NULL;

    if (parent->child == NULL)
    {
        parent->child = child;
    }
    else
    {
        nkView_t *lastChild = parent->child;

        while (lastChild->sibling != NULL)
        {
            lastChild = lastChild->sibling;
        }

        lastChild->sibling = child;
        child->prevSibling = lastChild; /* set the previous sibling */
    }
}

void nkView_RemoveChildView(nkView_t *parent, nkView_t *child)
{
    if (parent == NULL || child == NULL)
    {
        return;
    }

    nkView_t* prev = child->prevSibling;
    nkView_t* next = child->sibling;

    if (prev != NULL)
    {
        /* The child is in the middle or end of the list */
        prev->sibling = next;
    }
    else
    {
        /* The child is the first child of the parent */
        parent->child = next;
    }

    if (next != NULL)
    {
        /* The child was not the last in the list */
        next->prevSibling = prev;
    }

    /* Isolate the removed child */
    child->parent = NULL;
    child->sibling = NULL;
    child->prevSibling = NULL;
}

void nkView_RemoveView(nkView_t *view)
{
    if (view == NULL || view->parent == NULL)
    {
        return;
    }

    nkView_RemoveChildView(view->parent, view);    

}

void nkView_InsertView(nkView_t *parent, nkView_t *child, nkView_t *before)
{
    if (parent == NULL || child == NULL)
    {
        return;
    }

    if (before == NULL || parent->child == NULL)
    {
        nkView_AddChildView(parent, child);
        return;
    }

    if (before->parent != parent)
    {
        /* 'before' is not a child of 'parent', add as last child view */
        nkView_AddChildView(parent, child);
        return;
    }

    child->parent = parent;

    /* get the previous sibling */

    nkView_t *prevSibling = before->prevSibling;

    child->sibling = before;
    child->prevSibling = prevSibling;

    before->prevSibling = child;

    if (prevSibling != NULL)
    {
        prevSibling->sibling = child;
    }
    else 
    {
        /* 'before' was the first child, so we set the new child as the first */
        parent->child = child;
    }
    
}

void nkView_ReplaceView(nkView_t *oldView, nkView_t *newView)
{
    if (oldView == NULL || oldView->parent == NULL || newView == NULL)
    {
        return;
    }

    nkView_t *parent = oldView->parent;
    nkView_t *prev = oldView->prevSibling;
    nkView_t *next = oldView->sibling;

    newView->parent = parent;
    newView->sibling = next;
    newView->prevSibling = prev;

    /* Update the surrounding nodes to point to the new view */
    if (next != NULL)
    {
        next->prevSibling = newView;
    }

    if (prev != NULL)
    {
        prev->sibling = newView;
    }
    else
    {
        /* oldView was the first child */
        parent->child = newView;
    }

    /* Isolate the old view completely */
    oldView->parent = NULL;
    oldView->sibling = NULL;
    oldView->prevSibling = NULL;
}

nkView_t *nkView_NextViewInTree(nkView_t *view)
{
    if (view == NULL)
    {
        return NULL;
    }

    if (view->child != NULL)
    {
        return view->child;
    }
    
    while (view != NULL && view->sibling == NULL)
    {
        view = view->parent;
    }

    if (view != NULL)
    {
        return view->sibling;
    }

    return NULL;
}

nkView_t *nkView_PreviousViewInTree(nkView_t *view)
{
    if (view == NULL)
    {
        return NULL;
    }

    /* Use the new, fast pointer to get the previous sibling in O(1) time */
    nkView_t *prev_sibling = view->prevSibling;

    if (prev_sibling != NULL)
    {
        /* The previous node in a post-order traversal is the deepest,
            right-most descendant of the previous sibling. */
        return nkView_DeepestViewInTree(prev_sibling);
    }

    /* If there is no previous sibling, the previous node is our parent. */
    return view->parent;

}


nkView_t *nkView_DeepestViewInTree(nkView_t *view)
{
    if (view == NULL)
    {
        return NULL;
    }

    while (view->child != NULL)
    {
        view = view->child;
        while (view->sibling != NULL)
        {
            view = view->sibling;
        }
    }

    return view;
}

nkView_t *nkView_RootViewInTree(nkView_t *view)
{
    while (view != NULL && view->parent != NULL)
    {
        view = view->parent;
    }

    return view;
}

nkView_t *nkView_FirstChildView(nkView_t *view)
{
    if (view == NULL)
    {
        return NULL;
    }

    return view->child;
}

nkView_t *nkView_LastChildView(nkView_t *view)
{
    if (view == NULL)
    {
        return NULL;
    }

    nkView_t *lastChild = view->child;

    while (lastChild != NULL && lastChild->sibling != NULL)
    {
        lastChild = lastChild->sibling;
    }

    return lastChild;
}

nkView_t *nkView_NextSiblingView(nkView_t *view)
{
    if (view == NULL)
    {
        return NULL;
    }

    if (view->sibling != NULL)
    {
        return view->sibling;
    }

    return NULL;
}

nkView_t *nkView_PreviousSiblingView(nkView_t *view)
{
    if (view == NULL)
    {
        return NULL;
    }

    return view->prevSibling;
}

nkView_t* nkView_HitTest(nkView_t *view, float x, float y)
{

    bool inView = (
            (x >= view->frame.x)
        &&  (x <= (view->frame.x + view->frame.width))
        &&  (y >= view->frame.y) 
        &&  (y <= (view->frame.y + view->frame.height))
    );

    /* if not in this view, neither view nor any child can pass the test */
    if (!view || !inView)
    {
        return NULL;
    }

    /* search children top-down */
    nkView_t *child = nkView_LastChildView(view);

    while (child)
    {
        nkView_t *hitView = nkView_HitTest(child, x, y);

        if (hitView)
        {
            /* a child passed the hit test, so this takes precedence */
            return hitView;
        }

        child = nkView_PreviousSiblingView(child);
    }

    /* check if this view wants events */
    if (view->capturePointerHover || view->capturePointerMovement || view->capturePointerAction)
    {
        return view;
    }

    /* this view doesn't want events, so treat as invisible */
    return NULL;
}

/* places the view at the given frame, applying alignment and margin */
void nkView_PlaceView(nkView_t *view, nkRect_t frame)
{
    if (view == NULL)
    {
        return;
    }

    /* fit child to frame */
    switch (view->verticalAlignment)
    {
        case ALIGNMENT_TOP:
        {
            view->frame.height = fminf(view->sizeRequest.height, frame.height);
            view->frame.y = frame.y + view->margin.top;
        } break;

        case ALIGNMENT_MIDDLE:
        {
            view->frame.height = fminf(view->sizeRequest.height, frame.height);
            view->frame.y = frame.y + (frame.height - view->frame.height) / 2.0f;

            if (view->frame.y < frame.y + view->margin.top)
            {
                view->frame.y = frame.y + view->margin.top; /* Ensure it doesn't go above the margin */
            }
        } break;

        case ALIGNMENT_BOTTOM:
        {
            view->frame.height = fminf(view->sizeRequest.height, frame.height);
            view->frame.y = frame.y + (frame.height - view->frame.height);

            if (view->frame.y + view->frame.height > frame.y + frame.height - view->margin.bottom)
            {
                view->frame.y = frame.y + frame.height - view->frame.height - view->margin.bottom; /* Ensure it doesn't go below the margin */
            }
        } break;

        default:
        {
            view->frame.height = frame.height - view->margin.top - view->margin.bottom;
            view->frame.y = frame.y + view->margin.top;
        } break;
    }

    switch (view->horizontalAlignment)
    {
        case ALIGNMENT_LEFT:
        {
            view->frame.width = fminf(view->sizeRequest.width, frame.width);
            view->frame.x = frame.x + view->margin.left;
        } break;

        case ALIGNMENT_CENTER:
        {
            view->frame.width = fminf(view->sizeRequest.width, frame.width);
            view->frame.x = frame.x + (frame.width - view->frame.width) / 2.0f;

            if (view->frame.x < frame.x + view->margin.left)
            {
                view->frame.x = frame.x + view->margin.left; /* Ensure it doesn't go before the margin */
            }
        } break;

        case ALIGNMENT_RIGHT:
        {
            view->frame.width = fminf(view->sizeRequest.width, frame.width);
            view->frame.x = frame.x + (frame.width - view->frame.width);

            if (view->frame.x + view->frame.width > frame.x + frame.width - view->margin.right)
            {
                view->frame.x = frame.x + frame.width - view->frame.width - view->margin.right; /* Ensure it doesn't go after the margin */
            }
        } break;

        default:
        {
            view->frame.width = frame.width - view->margin.left - view->margin.right;
            view->frame.x = frame.x + view->margin.left;
        } break;
    }
    
}


/***************************************************************
** MARK: STATIC FUNCTIONS
***************************************************************/