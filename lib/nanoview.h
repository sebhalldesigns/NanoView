/***************************************************************
**
** NanoKit Library Header File
**
** File         :  nanoview.h
** Module       :  nanoview
** Author       :  SH
** Created      :  2025-02-23 (YYYY-MM-DD)
** License      :  MIT
** Description  :  NanoKit View API
**
***************************************************************/

#ifndef NANOVIEW_H
#define NANOVIEW_H

/***************************************************************
** MARK: INCLUDES
***************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include <nanodraw.h>

/***************************************************************
** MARK: CONSTANTS & MACROS
***************************************************************/

/***************************************************************
** MARK: TYPEDEFS
***************************************************************/

struct nkView_t; /* forward declaration */

typedef void (*ViewMeasureCallback_t)(struct nkView_t *view); 
typedef void (*ViewArrangeCallback_t)(struct nkView_t *view);
typedef void (*ViewDrawCallback_t)(struct nkView_t *view); 
typedef void (*ViewDestroyCallback_t)(struct nkView_t *view); /* called when view is destroyed */

typedef enum 
{
    ALIGNMENT_LEFT,
    ALIGNMENT_CENTER,
    ALIGNMENT_RIGHT
} nkHorizontalAlignment_t;

typedef enum 
{
    ALIGNMENT_TOP,
    ALIGNMENT_MIDDLE,
    ALIGNMENT_BOTTOM
} nkVerticalAlignment_t;

typedef enum
{
    STRETCH_NONE,
    STRETCH_HORIZONTAL,
    STRETCH_VERTICAL,
    STRETCH_BOTH
} nkStretchType_t;

typedef enum
{
    DOCK_POSITION_TOP,
    DOCK_POSITION_BOTTOM,
    DOCK_POSITION_LEFT,
    DOCK_POSITION_RIGHT
} nkDockPosition_t;

typedef struct
{
    size_t Row;
    size_t Column;
    size_t RowSpan;
    size_t ColumnSpan;
} nkGridLocation_t;

typedef struct nkView_t
{    

    const char* name; /* name of the view */

    nkRect_t frame; /* overwritten in layout phase */
    nkSize_t sizeRequest;

    /* tree structure handles */
    struct nkView_t *parent; /* can be NULL*/
    struct nkView_t *sibling; /* can be NULL*/
    struct nkView_t *prevSibling; /* can be NULL*/
    struct nkView_t *child; /* can be NULL*/

    /* Generic layout requests to parent */
    nkStretchType_t stretchType;
    nkHorizontalAlignment_t horizontalAlignment;
    nkVerticalAlignment_t verticalAlignment;

    /* Parent panel specific requests */
    nkDockPosition_t dockPosition;
    nkGridLocation_t gridLocation;
    nkRect_t canvasRect;
    
    /* callbacks */
    ViewMeasureCallback_t measureCallback;
    ViewArrangeCallback_t arrangeCallback;
    ViewDrawCallback_t drawCallback; /* called when view should be drawn */
    ViewDestroyCallback_t destroyCallback; /* called when view is destroyed */

    nkColor_t backgroundColor;

    void *data;
    size_t dataSize; /* size of the data in bytes */

} nkView_t;


/***************************************************************
** MARK: FUNCTION DEFS
***************************************************************/

bool nkView_Create(nkView_t *view, const char *name);

/* detroys view and all children */
void nkView_Destroy(nkView_t *view);

/* called by system on window resize */
void nkView_Layout(nkView_t *view);

/* called by system when view should be rendered */
void nkView_Draw(nkView_t *view);

/* VIEW TREE MANAGEMENT */

void nkView_AddChildView(nkView_t *parent, nkView_t *child);
void nkView_RemoveChildView(nkView_t *parent, nkView_t *child);
void nkView_RemoveView(nkView_t *view);
void nkView_InsertView(nkView_t *parent, nkView_t *child, nkView_t *before);
void nkView_ReplaceView(nkView_t *oldView, nkView_t *newView);


/* TREE TRAVERSAL */

nkView_t *nkView_NextViewInTree(nkView_t *view);
nkView_t *nkView_PreviousViewInTree(nkView_t *view);
nkView_t *nkView_DeepestViewInTree(nkView_t *view);
nkView_t *nkView_RootViewInTree(nkView_t *view);
nkView_t *nkView_FirstChildView(nkView_t *view);
nkView_t *nkView_LastChildView(nkView_t *view);
nkView_t *nkView_NextSiblingView(nkView_t *view);
nkView_t *nkView_PreviousSiblingView(nkView_t *view);


#endif /* NANOVIEW_H */