// Torsion TorqueScript IDE - Copyright (C) Sickhead Games, LLC
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#include "PreCompiled.h"
#include "AppPrefs.h"

#include <wx/file.h>
#include <wx/tokenzr.h>
#include <wx/docview.h>

#include "XmlFile.h"

#include "helper.h"

#ifdef _DEBUG 
   #define new DEBUG_NEW 
#endif 

const wxChar* AppPrefs::s_ReservedWords = 
   L"break case continue datablock package default else false function if for new or package return " \
   L"switch switch$ true %this while singleton local";

IMPLEMENT_CLASS(tsPrefsUpdateHint, wxObject)


AppPrefs::AppPrefs() :
	m_bDirty( true ),
	m_DefaultFont( 10, wxDEFAULT, wxNORMAL, wxNORMAL, false, L"Courier New" ),
   m_TabsAndSpaces( false ),
   m_LineBreaks( false ),
   m_LineNumbers( true ),
   m_Position( 0,0,0,0 ),
   m_Maximized( false ),
   m_CodeFolding( true ),
   m_TabWidth( 3 ),
   m_UseTabs( false ),
   m_LineWrap( true ),
   m_UseEdgeMarker( true ),
   m_CheckForUpdates( true ),
   m_ShowAllMods( false ),
   m_ShowAllFiles( true ),
   m_SyncProjectTree( true ),
   m_ProjectSashWidth( 0 ),
   m_BottomSashHeight( 0 ),
   m_LoadLastProject( true ),
   m_EdgeColumn( 80 )
{
}

bool AppPrefs::Load( const wxString& Path )
{
	wxFile File;
	if ( !File.Open( Path, wxFile::read ) ) {

      // This loads defaults.
      LoadFromString( L"" );
		return false;
	}

	size_t Length = File.Length();
    char* Buffer = new char[ Length+1 ];
	File.Read( Buffer, Length );
	Buffer[ Length ] = 0;
	LoadFromString( widen(Buffer).c_str() );
	delete [] Buffer;

   return true;
}

void AppPrefs::LoadFromString( const wxChar* Buffer )
{
   XmlFile Xml( Buffer );

	Xml.FindElem( L"TorsionPrefs" );
	Xml.IntoElem();

   wxString fontInfo = Xml.GetStringElem( L"DefaultFont", m_DefaultFont.GetNativeFontInfoDesc() );
   m_DefaultFont.SetNativeFontInfo( fontInfo );

   m_Reserved = Xml.GetStringElem( L"ReservedWords", s_ReservedWords );
	ReformatWords( m_Reserved );
   m_ReservedColor = Xml.GetColorAttrib( L"ReservedWords", L"color", wxColour( 0, 0, 255 ) );

   m_ExportsColor = Xml.GetColorElem( L"ExportsColor", wxColour( 0, 0, 128 ) );

   m_BgColor = Xml.GetColorElem( L"BgColor", wxColour( 255,255,255 ) );

   m_DefaultColor = Xml.GetColorElem( L"DefaultColor", wxColour( 0,0,0 ) );
   m_CommentColor = Xml.GetColorElem( L"CommentColor", wxColour( 0,128,0 ) );
   m_NumberColor = Xml.GetColorElem( L"NumberColor", wxColour( 0,0,0 ) );
   m_StringColor = Xml.GetColorElem( L"StringColor", wxColour( 128,0,128 ) );
   m_OperatorsColor = Xml.GetColorElem( L"OperatorsColor", wxColour( 128,128,0 ) );
   m_LocalsColor = Xml.GetColorElem( L"LocalsColor", wxColour( 0,128,128 ) );
   m_GlobalsColor = Xml.GetColorElem( L"GlobalsColor", wxColour( 196,92,0 ) );

   m_SelColor = Xml.GetColorElem( L"SelColor", wxColour( 255,255,255 ) );
   m_SelBgColor = Xml.GetColorElem( L"SelBgColor", wxColour( 49,106,197 ) );

   m_BraceMatchColor = Xml.GetColorElem( L"BraceMatchColor", wxColour( 0,0,0 ) );
   m_BraceMatchBgColor = Xml.GetColorElem( L"BraceMatchBgColor", wxColour( 240,240,240 ) );
   m_BraceMatchErrColor = Xml.GetColorElem( L"BraceMatchErrColor", wxColour( 197,0,0 ) );

   m_CalltipColor = Xml.GetColorElem( L"CalltipColor", wxColour( 0,0,0 ) );
   m_CalltipBgColor = Xml.GetColorElem( L"CalltipBgColor", wxColour( 255,255,225 ) );
   m_CalltipHiColor = Xml.GetColorElem( L"CalltipHiColor", wxColour( 0,128,128 ) );

   m_MarginColor = Xml.GetColorElem( L"MarginColor", wxColour( 240,240,240 ) );
   m_MarginTextColor = Xml.GetColorElem( L"MarginTextColor", wxColour( 0,0,0 ) );

   m_CodeCompletion = Xml.GetBoolElem( L"CodeCompletion", true );
   
   m_EnhancedCompletion = Xml.GetBoolElem( L"EnhancedCompletion", true );

   m_CodeFolding = Xml.GetBoolElem( L"CodeFolding", true );
   m_CodeFoldingColor = Xml.GetColorElem( L"CodeFoldingColor", wxColour( 128, 128, 128 ) );

   wxRect desktop( wxScreenDC().GetSize() );

   Xml.FindElem( L"Position" );
	m_Position = StringToRect( Xml.GetData().c_str() );
   {
      if ( !desktop.Inside( m_Position.GetTopLeft() ) )
         m_Position.SetTopLeft( wxPoint( 0, 0 ) );
      if ( m_Position.GetWidth() > desktop.GetWidth() )
         m_Position.SetWidth( desktop.GetWidth() );
      if ( m_Position.GetHeight() > desktop.GetHeight() )
         m_Position.SetHeight( desktop.GetHeight() );
   }
	Xml.ResetMainPos();
   m_Maximized = Xml.GetBoolElem( L"Maximized", false );

   m_ProjectSashWidth = Xml.GetIntElem( L"ProjectSashWidth", -1 );
   m_BottomSashHeight = Xml.GetIntElem( L"BottomSashHeight", -1 );

   m_FileHistory.Empty();
   Xml.GetArrayStringElems( m_FileHistory, L"FileHistory", L"File" );

   m_ProjectHistory.Empty();
   Xml.GetArrayStringElems( m_ProjectHistory, L"ProjectHistory", L"Project" );

   m_FindStrings.Empty();
   Xml.GetArrayStringElems( m_FindStrings, L"FindHistory", L"String" );

   m_FindTypes.Empty();
   if ( Xml.GetArrayStringElems( m_FindTypes, L"FindTypes", L"Type" ) < 1 )
      m_FindTypes.Add( L"*.cs;*.gui;*.mis;*.t2d" );

   m_FindPaths.Empty();
   Xml.GetArrayStringElems( m_FindPaths, L"FindPaths", L"Path" );
   FixupFindPaths( m_FindPaths );

   m_FindMatchCase = Xml.GetBoolElem( L"FindMatchCase", false );
   m_FindMatchWord = Xml.GetBoolElem( L"FindMatchWord", false );
   m_FindSearchUp = Xml.GetBoolElem( L"FindSearchUp", false );
   
   m_FindPos = Xml.GetPointElem( L"FindPos", wxDefaultPosition );
   if ( !desktop.Inside( m_FindPos ) )
      m_FindPos = wxDefaultPosition;

   m_FindSymbols.Empty();
   Xml.GetArrayStringElems( m_FindSymbols, L"FindSymbols", L"String" );

   m_LoadLastProject = Xml.GetBoolElem( L"LoadLastProject", true );
   m_LastProject = Xml.GetStringElem( L"LastProject", wxEmptyString );

   m_ScriptExts.Empty();
   wxString exts = Xml.GetStringElem( L"ScriptExtensions", L"cs,gui,mis,t2d" );
   SetScriptExtsString( exts );

   m_DSOExts.Empty();
   exts = Xml.GetStringElem( L"DSOExts", L"dso,edso" );
   SetDSOExtsString( exts );

   m_ExcludedFiles.Empty();
   if ( Xml.GetArrayStringElems( m_ExcludedFiles, L"ExcludedFileNames", L"Name" ) == -1 ) 
   {
      m_ExcludedFiles.Add( L".dll" );
      m_ExcludedFiles.Add( L".exe" );
      m_ExcludedFiles.Add( L".lnk" );
      m_ExcludedFiles.Add( L".torsion" );
      m_ExcludedFiles.Add( L".bak" );
      m_ExcludedFiles.Add( L".opt" );  // TODO: Change to options once we fix that
      m_ExcludedFiles.Add( L".exports" );
      m_ExcludedFiles.Add( L".tmp" );
      m_ExcludedFiles.Add( L".ilk" );
      m_ExcludedFiles.Add( L".pdb" );
   }

   m_ExcludedFolders.Empty();
   if ( Xml.GetArrayStringElems( m_ExcludedFolders, L"ExcludedFolderNames", L"Name" ) == -1 ) {

      m_ExcludedFolders.Add( L"_svn" );
      m_ExcludedFolders.Add( L".svn" );
      m_ExcludedFolders.Add( L"cvs" );
   }

   m_TextExts.Empty();
   if ( Xml.GetArrayStringElems( m_TextExts, L"TextExts", L"Ext" ) == -1 ) {

      /*
      TODO: Support opening text files!
      m_TextExts.Add( "txt" );
      m_TextExts.Add( "ini" );
      m_TextExts.Add( "log" );
      m_TextExts.Add( "readme" );
      m_TextExts.Add( "html" );
      m_TextExts.Add( "htm" );
      m_TextExts.Add( "hlsl" );
      m_TextExts.Add( "bat" );
      m_TextExts.Add( "xml" );
      */
   }

   m_UseTabs = Xml.GetBoolElem( L"UseTabs", false );
   m_TabWidth = Xml.GetIntElem( L"TabWidth", 3 );

   m_LineWrap = Xml.GetBoolElem( L"LineWrap", false );
   
   m_UseEdgeMarker = Xml.GetBoolElem( L"EdgeMarker", true );
   m_EdgeColumn = Xml.GetIntElem( L"EdgeMarkerColumn", 80 );
   m_EdgeMarkerColor = Xml.GetColorElem( L"EdgeMarkerColor", wxColour( 240,240,240 ) );

   m_TabsAndSpaces = Xml.GetBoolElem( L"ShowTabsAndSpaces", false );
   m_LineBreaks = Xml.GetBoolElem( L"ShowLineBreaks", false );
   m_LineNumbers = Xml.GetBoolElem( L"ShowLineNumbers", true );

   m_ShowAllMods        = Xml.GetBoolElem( L"ShowAllMods", false );
   m_ShowAllFiles       = Xml.GetBoolElem( L"ShowAllFiles", true );
   m_SyncProjectTree    = Xml.GetBoolElem( L"SyncProjectTree", true );

   m_CheckForUpdates = Xml.GetBoolElem( L"CheckForUpdates", true );

   m_ToolCommands.Clear();
   if ( Xml.FindElem( L"ExternalTools" ) )
   {
      Xml.IntoElem();

      while ( Xml.FindElem( L"Tool" ) && Xml.IntoElem() )
      {
         ToolCommand cmd;
         cmd.SetTitle( Xml.GetStringElem( L"Title", L"(Empty)" ) );
         cmd.SetCmd( Xml.GetStringElem( L"Command", wxEmptyString ) );
         cmd.SetArgs( Xml.GetStringElem( L"Arguments", wxEmptyString ) );
         cmd.SetDir( Xml.GetStringElem( L"Directory", wxEmptyString ) );
         m_ToolCommands.Add( cmd );
         Xml.OutOfElem();
      }

      Xml.OutOfElem();
   }
   else
   {
      ToolCommand cmd;
      cmd.SetTitle( L"(Empty)" );
      m_ToolCommands.Add( cmd );
      m_ToolCommands.Add( cmd );
      m_ToolCommands.Add( cmd );
      m_ToolCommands.Add( cmd );
   }

   m_bDirty = false;
}

bool AppPrefs::Save( const wxString& Path )
{
	wxFile File;
	if ( !File.Open( Path, wxFile::write ) ) {
		return false;
	}

	XmlFile Xml;
	Xml.AddElem( L"TorsionPrefs" );
	Xml.IntoElem();

   Xml.AddElem( L"DefaultFont", MCD_CSTR(m_DefaultFont.GetNativeFontInfoDesc()) );

   Xml.AddElem( L"ReservedWords", MCD_CSTR(m_Reserved ));
	Xml.AddAttrib( L"color", MCD_CSTR(Xml.ColorToString( m_ReservedColor) ) );

	Xml.AddElem( L"ExportsColor", MCD_CSTR(Xml.ColorToString( m_ExportsColor) ) );

   Xml.AddColorElem( L"BgColor", m_BgColor );

   Xml.AddColorElem( L"DefaultColor", m_DefaultColor );
   Xml.AddColorElem( L"CommentColor", m_CommentColor );
   Xml.AddColorElem( L"StringColor", m_StringColor );
   Xml.AddColorElem( L"NumberColor", m_NumberColor );
   Xml.AddColorElem( L"OperatorsColor", m_OperatorsColor );
   Xml.AddColorElem( L"LocalsColor", m_LocalsColor );
   Xml.AddColorElem( L"GlobalsColor", m_GlobalsColor );

   Xml.AddColorElem( L"SelColor", m_SelColor );
   Xml.AddColorElem( L"SelBgColor", m_SelBgColor );

   Xml.AddColorElem( L"BraceMatchColor", m_BraceMatchColor );
   Xml.AddColorElem( L"BraceMatchBgColor", m_BraceMatchBgColor );
   Xml.AddColorElem( L"BraceMatchErrColor", m_BraceMatchErrColor );

   Xml.AddColorElem( L"CalltipColor", m_CalltipColor );
   Xml.AddColorElem( L"CalltipBgColor", m_CalltipBgColor );
   Xml.AddColorElem( L"CalltipHiColor", m_CalltipHiColor );

   Xml.AddColorElem( L"MarginColor", m_MarginColor );
   Xml.AddColorElem( L"MarginTextColor", m_MarginTextColor );

   Xml.AddBoolElem( L"CodeFolding", m_CodeFolding );
   Xml.AddColorElem( L"CodeFoldingColor", m_CodeFoldingColor );

   Xml.AddBoolElem( L"CodeCompletion", m_CodeCompletion );

   Xml.AddBoolElem( L"EnhancedCompletion", m_EnhancedCompletion );
   
   Xml.AddElem( L"Position", MCD_CSTR(RectToString( m_Position )) );
   Xml.AddBoolElem( L"Maximized", m_Maximized );

   Xml.AddIntElem( L"ProjectSashWidth", m_ProjectSashWidth );
   Xml.AddIntElem( L"BottomSashHeight", m_BottomSashHeight );

   Xml.AddBoolElem( L"UseTabs", GetUseTabs() );
   Xml.AddIntElem( L"TabWidth", GetTabWidth() );
   Xml.AddBoolElem( L"LineWrap", GetLineWrap() );

   Xml.AddBoolElem( L"EdgeMarker", GetEdgeMarker() );
   Xml.AddIntElem( L"EdgeMarkerColumn", GetEdgeMarkerCol() );
   Xml.AddColorElem( L"EdgeMarkerColor", GetEdgeMarkerColor() );

   Xml.AddBoolElem( L"ShowTabsAndSpaces", GetTabsAndSpaces() );
   Xml.AddBoolElem( L"ShowLineBreaks", GetLineBreaks() );
   Xml.AddBoolElem( L"ShowLineNumbers", GetLineNumbers() );

   Xml.AddArrayStringElems( L"FileHistory", L"File", m_FileHistory );
   Xml.AddArrayStringElems( L"ProjectHistory", L"Project", m_ProjectHistory );

   Xml.AddArrayStringElems( L"FindHistory", L"String", m_FindStrings );
   Xml.AddArrayStringElems( L"FindTypes", L"Type", m_FindTypes );
   Xml.AddArrayStringElems( L"FindPaths", L"Path", m_FindPaths );

   Xml.AddBoolElem( L"FindMatchCase", m_FindMatchCase );
   Xml.AddBoolElem( L"FindMatchWord", m_FindMatchWord );
   Xml.AddBoolElem( L"FindSearchUp", m_FindSearchUp );

   Xml.AddPointElem( L"FindPos", m_FindPos );

   Xml.AddArrayStringElems( L"FindSymbols", L"String", m_FindSymbols );

   Xml.AddBoolElem( L"LoadLastProject", m_LoadLastProject );
   Xml.AddElem( L"LastProject", MCD_CSTR(m_LastProject) );

   Xml.AddElem( L"ScriptExtensions", MCD_CSTR(GetScriptExtsString() ));
   Xml.AddElem( L"DSOExts", MCD_CSTR(GetDSOExtsString() ));
   Xml.AddArrayStringElems( L"ExcludedFileNames", L"Name", m_ExcludedFiles );
   Xml.AddArrayStringElems( L"ExcludedFolderNames", L"Name", m_ExcludedFolders );
   Xml.AddArrayStringElems( L"TextExts", L"Ext", m_TextExts );

   Xml.AddBoolElem( L"ShowAllMods", m_ShowAllMods );
   Xml.AddBoolElem( L"ShowAllFiles", m_ShowAllFiles );
   Xml.AddBoolElem( L"SyncProjectTree", m_SyncProjectTree );

   Xml.AddBoolElem( L"CheckForUpdates", m_CheckForUpdates );

   Xml.AddElem( L"ExternalTools" );
	Xml.IntoElem();
   for ( size_t i=0; i < m_ToolCommands.GetCount(); i++ ) 
   {
      const ToolCommand& cmd = m_ToolCommands[i];

      Xml.AddElem( L"Tool" );
	   Xml.IntoElem();

         Xml.AddElem( L"Title", MCD_CSTR(cmd.GetTitle() ));
         Xml.AddElem( L"Command", MCD_CSTR(cmd.GetCmd() ));
         Xml.AddElem( L"Arguments", MCD_CSTR(cmd.GetArgs() ));
         Xml.AddElem( L"Directory", MCD_CSTR(cmd.GetDir() ));

      Xml.OutOfElem();
   }
   Xml.OutOfElem();

   std::string Buffer( narrow(Xml.GetDoc() ));
	File.Write( Buffer.c_str(), Buffer.length() );

   return true;
}

bool AppPrefs::SaveIfDirty( const wxString& Path )
{
	if ( IsDirty() ) {
		return Save( Path );
	}
	return true;
}

static int CmpStringOrderNoCase( const wxString& first, const wxString& second )
{
    return first.CmpNoCase( second );
}

void AppPrefs::ReformatWords( wxString& Words )
{
	wxStringTokenizer Tokenizer( Words, L"\t\r\n ,;", wxTOKEN_STRTOK );
   wxArrayString Sorted;
	wxString Token;
	while ( Tokenizer.HasMoreTokens() ) {

		Token = Tokenizer.GetNextToken();
		Token.Trim( true );
		Token.Trim( false );
      Sorted.Add( Token );
	}

   Sorted.Sort( CmpStringOrderNoCase );

   Words.Empty();
   for ( int i=0; i < Sorted.GetCount(); i++ ) {
      Words << ( i != 0 ? L" " : L"" ) << Sorted[i];
   }
}

wxString AppPrefs::ColorToString( const wxColour& Color )
{
	wxString Result;
   Result << Color.Red() << L"," << Color.Green() << L"," << Color.Blue();
	return Result;
}

wxColour AppPrefs::StringToColor( const wxString& Color )
{
	wxStringTokenizer Tokenizer( Color, L"\t\r\n ,;", wxTOKEN_STRTOK );

	wxString Token = Tokenizer.GetNextToken();
	Token.Trim( true );
	Token.Trim( false );
	long Red = 0;
	Token.ToLong( &Red );
	Red = wxMin( 255, wxMax( 0, Red ) );
	
	Token = Tokenizer.GetNextToken();
	Token.Trim( true );
	Token.Trim( false );
	long Green = 0;
	Token.ToLong( &Green );
	Green = wxMin( 255, wxMax( 0, Green ) );

	Token = Tokenizer.GetNextToken();
	Token.Trim( true );
	Token.Trim( false );
	long Blue = 0;
	Token.ToLong( &Blue );
	Blue = wxMin( 255, wxMax( 0, Blue ) );

	return wxColour( Red, Green, Blue );
}

wxString AppPrefs::RectToString( const wxRect& Rect )
{
	wxString Result;
   Result << Rect.GetLeft() << L"," << Rect.GetTop() << L"," << Rect.GetWidth() << L"," << Rect.GetHeight();
	return Result;
}

wxRect AppPrefs::StringToRect( const wxString& Rect )
{
	wxStringTokenizer Tokenizer( Rect, L"\t\r\n ,;", wxTOKEN_STRTOK );

	wxString Token = Tokenizer.GetNextToken();
	Token.Trim( true );
	Token.Trim( false );
	long Left = 0;
	Token.ToLong( &Left );

	Token = Tokenizer.GetNextToken();
	Token.Trim( true );
	Token.Trim( false );
	long Top = 0;
	Token.ToLong( &Top );

	Token = Tokenizer.GetNextToken();
	Token.Trim( true );
	Token.Trim( false );
	long Width = 0;
	Token.ToLong( &Width );

   Token = Tokenizer.GetNextToken();
	Token.Trim( true );
	Token.Trim( false );
	long Height = 0;
	Token.ToLong( &Height );

   return wxRect( Left, Top, Width, Height );
}

void AppPrefs::GetFileHistory( wxFileHistory* history ) const
{
   wxASSERT( history );

   // Note we add here in reverse because AddFileToHistory 
   // inserts items into the top.
   for ( size_t i=m_FileHistory.GetCount(); i > 0; i-- ) 
      history->AddFileToHistory( m_FileHistory[ i - 1 ] );
}

void AppPrefs::SetFileHistory( wxFileHistory* history )
{
   wxASSERT( history );

   m_FileHistory.Empty();
   for ( size_t i=0; i < history->GetCount(); i++ )
      m_FileHistory.Add( history->GetHistoryFile( i ) );

   m_bDirty = true;
}

void AppPrefs::GetProjectHistory( wxFileHistory* history ) const
{
   wxASSERT( history );

   // Note we add here in reverse because AddFileToHistory 
   // inserts items into the top.
   for ( size_t i=m_ProjectHistory.GetCount(); i > 0; i-- ) 
      history->AddFileToHistory( m_ProjectHistory[ i - 1 ] );
}

void AppPrefs::SetProjectHistory( wxFileHistory* history )
{
   wxASSERT( history );

   m_ProjectHistory.Empty();
   for ( size_t i=0; i < history->GetCount(); i++ )
      m_ProjectHistory.Add( history->GetHistoryFile( i ) );

   m_bDirty = true;
}

bool AppPrefs::IsScriptFile( const wxString& file ) const
{
   wxFileName ext( file );
   return m_ScriptExts.Index( ext.GetExt(), wxFileName::IsCaseSensitive() ) != wxNOT_FOUND;
}

bool AppPrefs::IsTextFile( const wxString& file ) const
{
   wxFileName ext( file );
   return m_TextExts.Index( ext.GetExt(), wxFileName::IsCaseSensitive() ) != wxNOT_FOUND;
}

bool AppPrefs::IsExcludedFile( const wxString& file ) const
{
   wxFileName fixed( file );

   wxString ext;
   ext << L"." << fixed.GetExt();

   return 
      m_ExcludedFiles.Index( fixed.GetFullName(), wxFileName::IsCaseSensitive() ) != wxNOT_FOUND ||
      m_ExcludedFiles.Index( ext, wxFileName::IsCaseSensitive() ) != wxNOT_FOUND;
}

bool AppPrefs::IsExcludedFolder( const wxString& name ) const
{
   return m_ExcludedFolders.Index( name, wxFileName::IsCaseSensitive() ) != wxNOT_FOUND;
}

void AppPrefs::SetFindStrings( const wxArrayString& strings ) 
{ 
   m_FindStrings = strings; 
   m_bDirty = true;
}

void AppPrefs::SetFindTypes( const wxArrayString& types ) 
{ 
   m_FindTypes = types;
   m_bDirty = true;
}

void AppPrefs::SetFindPaths( const wxArrayString& paths ) 
{ 
   m_FindPaths = paths; 
   m_bDirty = true;
}

void AppPrefs::SetFindPos( const wxPoint& point )
{
   if ( point != m_FindPos )
   {
      m_FindPos = point;
      m_bDirty = true;
   }
}

void AppPrefs::FixupFindPaths( wxArrayString& paths )
{
   if ( paths.GetCount() > 20 )
      paths.SetCount( 20 );

   if ( paths.Index( L"Project", false ) != wxNOT_FOUND ) 
      paths.RemoveAt( paths.GetCount() - 1 );
   if ( paths.Index( L"Base Directory", false ) != wxNOT_FOUND ) 
      paths.RemoveAt( paths.GetCount() - 1 );

   if ( paths.Index( L"Project", false ) == wxNOT_FOUND )
      paths.Add( L"Project" );
   if ( paths.Index( L"Base Directory", false ) == wxNOT_FOUND )
      paths.Add( L"Base Directory" );
}

void AppPrefs::SetFindSymbols( const wxArrayString& strings ) 
{ 
   m_FindSymbols = strings; 
   m_bDirty = true;
}

int AppPrefs::GetStringsFromCombo( wxComboBox* combo, wxArrayString& output )
{
   wxASSERT( combo );

   int count = 0;
   for ( int i=0; i < combo->GetCount(); i++ ) {
      output.Add( combo->GetString( i ) );
      ++count;
   }

   return count;
}

void AppPrefs::AddStringsToCombo( wxComboBox* combo, const wxArrayString& strings )
{
   wxASSERT( combo );
   for ( int i=0; i < strings.GetCount(); i++ ) {
      
      if ( strings[i].IsEmpty() )
         continue;

      combo->Append( strings[i] ); 
   }
   if ( strings.GetCount() > 0 )
      combo->SetValue( combo->GetString( 0 ) );
}

bool AppPrefs::SetScriptExtsString( const wxString& value )
{
	wxStringTokenizer toker( value, L",;", wxTOKEN_STRTOK );
   wxArrayString exts;
   wxString ext;
	while ( toker.HasMoreTokens() ) {

      ext = toker.GetNextToken();
		ext.Trim( true );
		ext.Trim( false );
      
      if ( ext.IsEmpty() )
         continue;

      if ( ext[0] == '.' )
         ext.Remove( 0, 1 );

      if ( !ext.IsEmpty() )
         exts.Add( ext );
	}

   // Sort the extensions alphabetically.
   //exts.Sort();

   if ( exts != m_ScriptExts )
   {
      m_ScriptExts = exts;
      m_bDirty = true;
      return true;
   }

   return false; 
}

wxString AppPrefs::GetDefaultScriptExtension() const
{
   wxString ext( L"cs" );
   if ( m_ScriptExts.GetCount() > 0 )
      ext = m_ScriptExts[0];
   return ext;
}

wxString AppPrefs::GetScriptExtsString() const
{
   wxString exts;

   for ( int i=0; i < m_ScriptExts.GetCount(); i++ )
      exts << m_ScriptExts[i] << L"; ";

   exts.RemoveLast(2);

   return exts;
}

bool AppPrefs::SetExcludedFiles( const wxString& exclude )
{
	wxStringTokenizer toker( exclude, L",;", wxTOKEN_STRTOK );
   wxArrayString files;
   wxString file;
	while ( toker.HasMoreTokens() ) {

      file = toker.GetNextToken();
		file.Trim( true );
		file.Trim( false );
		files.Add( file );
	}

   if ( files != m_ExcludedFiles )
   {
      m_ExcludedFiles = files;
      m_bDirty = true;
      return true;
   }

   return false; 
}

wxString AppPrefs::GetExcludedFilesString() const
{
   wxString exclude;

   for ( int i=0; i < m_ExcludedFiles.GetCount(); i++ )
      exclude << m_ExcludedFiles[i] << L"; ";

   exclude.RemoveLast(2);

   return exclude;
}

bool AppPrefs::SetExcludedFolders( const wxString& exclude )
{
	wxStringTokenizer toker( exclude, L",;", wxTOKEN_STRTOK );
   wxArrayString folders;
   wxString folder;
	while ( toker.HasMoreTokens() ) {

      folder = toker.GetNextToken();
		folder.Trim( true );
		folder.Trim( false );
		folders.Add( folder );
	}

   if ( folders != m_ExcludedFolders )
   {
      m_ExcludedFolders = folders;
      m_bDirty = true;
      return true;
   }

   return false; 
}

wxString AppPrefs::GetExcludedFoldersString() const
{
   wxString exclude;

   for ( int i=0; i < m_ExcludedFolders.GetCount(); i++ )
      exclude << m_ExcludedFolders[i] << L"; ";

   exclude.RemoveLast(2);

   return exclude;
}

bool AppPrefs::SetDSOExtsString( const wxString& value )
{
	wxStringTokenizer toker( value, L",;", wxTOKEN_STRTOK );
   wxArrayString exts;
   wxString ext;
	while ( toker.HasMoreTokens() ) {

      ext = toker.GetNextToken();
		ext.Trim( true );
		ext.Trim( false );
      
      if ( ext.IsEmpty() )
         continue;

      if ( ext[0] == '.' )
         ext.Remove( 0, 1 );

      if ( !ext.IsEmpty() )
         exts.Add( ext );
	}

   // Sort the extensions alphabetically.
   //exts.Sort();

   if ( exts != m_ScriptExts )
   {
      m_DSOExts = exts;
      m_bDirty = true;
      return true;
   }

   return false; 
}

wxString AppPrefs::GetDSOExtsString() const
{
   wxString exts;

   for ( int i=0; i < m_DSOExts.GetCount(); i++ )
      exts << m_DSOExts[i] << L"; ";

   exts.RemoveLast(2);

   return exts;
}

bool AppPrefs::IsDSOExt( const wxString& ext ) const
{
   for ( int i=0; i < m_DSOExts.GetCount(); i++ )
   {
      if ( m_DSOExts[i].IsSameAs( ext, wxFileName::IsCaseSensitive() ) )
         return true;
   }

   return false;
}

bool AppPrefs::IsDSO( const wxString& file ) const
{
   wxFileName fname( file );
   return IsDSOExt( fname.GetExt() );
}

wxString AppPrefs::GetDSOForScript( const wxString& script ) const
{
   for ( int i=0; i < m_DSOExts.GetCount(); i++ )
   {
      wxString dsofile;
      dsofile << script << '.' << m_DSOExts[i];
      if ( wxFileName::FileExists( dsofile ) )
         return dsofile;
   }

   return wxEmptyString;
}

bool AppPrefs::SetToolCommands( const ToolCommandArray& cmds )
{
   // Check to see if we're dirty...
   bool dirty = true;
   if ( cmds.GetCount() == m_ToolCommands.GetCount() )
   {
      dirty = false;

      for ( int i=0; i < cmds.GetCount(); i++ ) 
      {
         if ( cmds[i] != m_ToolCommands[i] )
         {
            dirty = true;
            break;
         }
      }
   }

   if ( dirty ) 
   {
      m_ToolCommands = cmds;
      m_bDirty = true;
      return true;
   }

   return false;
}



