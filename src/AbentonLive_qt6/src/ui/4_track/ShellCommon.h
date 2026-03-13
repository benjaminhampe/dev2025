#pragma once
#include "LiveSkin.h"
#include "ImageButton.h"

// ============================================================================
struct ShellCommon
// ============================================================================
{
   static ImageButton* createEnableButton( const LiveSkin& skin, QWidget* parent ); // Bypass button
   static ImageButton* createMoreButton  ( const LiveSkin& skin, QWidget* parent ); // More button
   static ImageButton* createEditorButton( const LiveSkin& skin, QWidget* parent ); // Show/hide (plugin) editor window button
   static ImageButton* createUpdateButton( const LiveSkin& skin, QWidget* parent ); // UpdateFrom button
   static ImageButton* createSaveButton  ( const LiveSkin& skin, QWidget* parent ); // Save button
};