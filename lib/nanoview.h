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

typedef enum
{
    HOVER_BEGIN,
    HOVER_END
} nkPointerHover_t;

typedef enum
{
    POINTER_EVENT_BEGIN,
    POINTER_EVENT_END,
    POINTER_EVENT_DOUBLE,
    POINTER_EVENT_DRAG,
    POINTER_EVENT_CANCEL
} nkPointerEvent_t;

typedef enum
{
    NK_POINTER_ACTION_PRIMARY           = 0x01,
    NK_POINTER_ACTION_SECONDARY         = 0x02,
    NK_POINTER_ACTION_TERTIARY          = 0x03,
    NK_POINTER_ACTION_EXTENDED_1        = 0x04,
    NK_POINTER_ACTION_EXTENDED_2        = 0x05
} nkPointerAction_t;

typedef void (*ViewMeasureCallback_t)(struct nkView_t *view, nkDrawContext_t *context); 
typedef void (*ViewArrangeCallback_t)(struct nkView_t *view, nkDrawContext_t *context);
typedef void (*ViewDrawCallback_t)(struct nkView_t *view, nkDrawContext_t *context);
typedef void (*ViewDestroyCallback_t)(struct nkView_t *view); /* called when view is destroyed */

typedef void (*PointerHoverCallback_t)(struct nkView_t *view, nkPointerHover_t event);
typedef void (*PointerMovementCallback_t)(struct nkView_t *view, float x, float y); /* x, y in window coords */
typedef void (*PointerActionCallback_t)(struct nkView_t *view, nkPointerAction_t action, nkPointerEvent_t event, float x, float y); /* x, y in window coords */
typedef void (*ScrollCallback_t)(struct nkView_t *view, float delta);

typedef enum 
{
    ALIGNMENT_STRETCH,
    ALIGNMENT_LEFT,
    ALIGNMENT_CENTER,
    ALIGNMENT_RIGHT
} nkHorizontalAlignment_t;

typedef enum 
{
    ALIGNMENT_FILL,
    ALIGNMENT_TOP,
    ALIGNMENT_MIDDLE,
    ALIGNMENT_BOTTOM
} nkVerticalAlignment_t;

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
    nkThickness_t margin; 

    bool clipToBounds;

    /* tree structure handles */
    struct nkView_t *parent; /* can be NULL*/
    struct nkView_t *sibling; /* can be NULL*/
    struct nkView_t *prevSibling; /* can be NULL*/
    struct nkView_t *child; /* can be NULL*/

    /* event capture flags */
    bool capturePointerHover;
    bool capturePointerMovement; /* capture move events */
    bool capturePointerAction;
    bool captureScroll;

    /* Generic layout requests to parent */
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
    PointerHoverCallback_t pointerHoverCallback; /* called when pointer enters and exits the view */
    PointerMovementCallback_t pointerMovementCallback; /* called when pointer moves over the view */
    PointerActionCallback_t pointerActionCallback; /* called when pointer events occur */
    ScrollCallback_t scrollCallback; /* called when scroll events occur */

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

/* VIEW TREE USAGE */
void nkView_LayoutTree(nkView_t *root, nkSize_t size, nkDrawContext_t *drawContext);
void nkView_LayoutSubtree(nkView_t *view, nkDrawContext_t *drawContext);
void nkView_RenderTree(nkView_t *root, nkDrawContext_t *drawContext);
void nkView_ProcessPointerMovement(nkView_t *root, float x, float y, nkView_t **hotView, nkView_t *activeView, nkPointerAction_t activeAction);
void nkView_ProcessPointerAction(nkView_t *root, nkPointerAction_t action, nkPointerEvent_t event, float x, float y, nkView_t *hotView, nkView_t **activeView, nkPointerAction_t *activeAction);
void nkView_ProcessScroll(nkView_t *root, float delta, nkView_t *hotView);

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
size_t nkView_GetDepthInTree(nkView_t *view);

/* HIT TESTING */
nkView_t *nkView_HitTest(nkView_t *view, float x, float y); /* in window co-ordinates */

/* LAYOUT */
void nkView_PlaceView(nkView_t *view, nkRect_t frame); /* places the view at the given frame, applying alignment and margin */

#endif /* NANOVIEW_H */