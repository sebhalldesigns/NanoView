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

    
    view->parent = NULL;
    view->sibling = NULL;
    view->prevSibling = NULL;
    view->child = NULL;

    view->stretchType = STRETCH_NONE;
    view->horizontalAlignment = ALIGNMENT_CENTER;
    view->verticalAlignment = ALIGNMENT_MIDDLE;

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
    return;
}

void nkView_Layout(nkView_t *view)
{
    
}

void nkView_Draw(nkView_t *view)
{

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



/***************************************************************
** MARK: STATIC FUNCTIONS
***************************************************************/
