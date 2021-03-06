/////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/frame.h
// Purpose:
// Author:      Robert Roebling
// Id:          $Id: frame.h,v 1.75 2005/08/02 22:57:55 MW Exp $
// Copyright:   (c) 1998 Robert Roebling, Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifndef __GTKFRAMEH__
#define __GTKFRAMEH__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "frame.h"
#endif

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxMDIChildFrame;
class WXDLLIMPEXP_CORE wxMDIClientWindow;
class WXDLLIMPEXP_CORE wxMenu;
class WXDLLIMPEXP_CORE wxMenuBar;
class WXDLLIMPEXP_CORE wxToolBar;
class WXDLLIMPEXP_CORE wxStatusBar;

//-----------------------------------------------------------------------------
// wxFrame
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxFrame : public wxFrameBase
{
public:
    // construction
    wxFrame() { Init(); }
    wxFrame(wxWindow *parent,
               wxWindowID id,
               const wxString& title,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = wxDEFAULT_FRAME_STYLE,
               const wxString& name = wxFrameNameStr)
    {
        Init();

        Create(parent, id, title, pos, size, style, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& title,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxDEFAULT_FRAME_STYLE,
                const wxString& name = wxFrameNameStr);

    virtual ~wxFrame();

#if wxUSE_STATUSBAR
    virtual void PositionStatusBar();

    virtual wxStatusBar* CreateStatusBar(int number = 1,
                                         long style = wxST_SIZEGRIP|wxFULL_REPAINT_ON_RESIZE,
                                         wxWindowID id = 0,
                                         const wxString& name = wxStatusLineNameStr);
                                         
    void SetStatusBar(wxStatusBar *statbar);
#endif // wxUSE_STATUSBAR

#if wxUSE_TOOLBAR
    virtual wxToolBar* CreateToolBar(long style = -1,
                                     wxWindowID id = -1,
                                     const wxString& name = wxToolBarNameStr);
    void SetToolBar(wxToolBar *toolbar);
#endif // wxUSE_TOOLBAR
    
    wxPoint GetClientAreaOrigin() const { return wxPoint(0, 0); }

    // implementation from now on
    // --------------------------

    // GTK callbacks
    virtual void GtkOnSize( int x, int y, int width, int height );
    virtual void OnInternalIdle();

    bool          m_menuBarDetached;
    int           m_menuBarHeight;
    bool          m_toolBarDetached;

protected:
    // common part of all ctors
    void Init();

    // override wxWindow methods to take into account tool/menu/statusbars
    virtual void DoSetClientSize(int width, int height);
    virtual void DoGetClientSize( int *width, int *height ) const;

#if wxUSE_MENUS_NATIVE

    virtual void DetachMenuBar();
    virtual void AttachMenuBar(wxMenuBar *menubar);

public:
    // Menu size is dynamic now, call this whenever it might change.
    void UpdateMenuBarSize();

#endif // wxUSE_MENUS_NATIVE

    DECLARE_DYNAMIC_CLASS(wxFrame)
};

#endif // __GTKFRAMEH__
