/*
 *  PresetController.h
 *
 *  Copyright (c) 2001-2019 Nick Dowell
 *  Copyright (c) 2022-2023 Benjamin Hampe <benjaminhampe@gmx.de>
 *
 *  This file is part of amsynth.
 *
 *  amsynth is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  amsynth is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with amsynth.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once
#include "Preset.h"
#include "UpdateListener.h"

/*

#include <string>
#include <vector>
#include <array>

#ifndef DE_DEBUG
#define DE_DEBUG(x) (x)
#endif

struct Bank
{
   std::string mName;
   std::string mUri;
   bool mIsReadOnly = true;
   std::vector< Preset > mPresets;
   //Preset* mCurrentPreset = nullptr;
   int mCurrentPresetNo = -1;
   UpdateListener* mUpdateListener = nullptr;

   void notify() { if (mUpdateListener) mUpdateListener->updateParameters(); }

   bool hasPreset( std::string const & name ) const
   {
      auto it = std::find_if( mPresets.begin(), mPresets.end(),
         [&] ( Preset const & cached ) { return cached.getName() == name; } );
      if ( it == mPresets.end() )
      {
         return false;
      }
      else
      {
         return true;
      }
   }

   // Selects a Preset and makes it current, updating everything as necessary.
   // If the requested preset does not exist, then the request is ignored, and
   // an error value is returned.
   bool selectPreset( int const presetIndex )
   {
      if (presetIndex < 0 || presetIndex >= int(mPresets.size()) )
      {
         printf("Bank[%s].selectPreset(%d) :: Got invalid preset index.\n", mName.c_str(), presetIndex );
         fflush( stdout );
         return false;
      }

      mCurrentPresetNo = presetIndex;
      //mCurrentPreset = getPreset( mCurrentPresetNo );
      notify();
      //clearChangeBuffers ();
      return 0;
   }

   // access presets in the memory bank
   Preset& getPreset( int presetIndex )
   {
      if (presetIndex < 0 || presetIndex >= int(mPresets.size()) )
      {
         std::ostringstream o;
         o << "Bank[" << mName <<"].getPreset(" << presetIndex << ") :: Got invalid preset index.\n";
         std::string e = o.str();
         printf( e.c_str() ); fflush( stdout );
         throw std::runtime_error( e );
      }

      return mPresets[ size_t( presetIndex ) ];
   }

   // returns the preset currently being edited
   Preset& getCurrentPreset()
   {
      return getPreset( mCurrentPresetNo );
   }

   //bool	isCurrentPresetModified() { return !currentPreset.isEqual(presets[currentPresetNo]); }

   // Commit the current preset to memory
   //void	commitPreset		() { presets[currentPresetNo] = currentPreset; notify(); }

   // Resets all parameters to default value and clears the name.
   void clearPreset()
   {
      //loadPresets();
      //currentPreset = blankPreset;
      //commitPreset();
      //savePresets();
      //clearChangeBuffers();
   }

};

struct BankManager
{
   std::vector< Bank > mBanks;
   int m_currentBankIndex;
   int m_currentPresetIndex;

   UpdateListener* mUpdateListener = nullptr;

   BankManager() {}
   ~BankManager() {} // clearChangeBuffers(); }

   // Assign a new preset without changing `currentPresetNo` - useful if not
   // using the standard bank management mechanism.
   void	setCurrentPreset	(const Preset &preset) { currentPreset = preset; }


   // Manages undo/redo for changes to current preset.
   void	pushParamChange		( const Param param, const float value );
   void	undoChange			();
   void	redoChange			();

   // Randomises the current preset.
   void	randomiseCurrentPreset	();

   // Saves the active preset to the filename filename
   int		exportPreset		(const std::string filename);
   int		importPreset		(const std::string filename);

   // Loading & Saving of bank files - NOT REALTIME SAFE
   int		loadPresets			(const char *filename = NULL);
   int		savePresets			(const char *filename = NULL);

   // Switch bank at runtime - safe to call on audio thread
   void	selectBank			(int bankNumber);

   void	setUpdateListener	(UpdateListener & ul) { updateListener = &ul; }

    int		getCurrPresetNumber	() { return currentPresetNo; }

   const std::string & getFilePath() { return bank_file; }

   static const std::vector<BankInfo> & getPresetBanks();
   static void rescanPresetBanks();

    static std::string getUserBanksDirectory();

   void notify() { if (updateListener) updateListener->updateParameters(); }

private:

   class ChangeData {
      public:
            virtual ~ChangeData() {};
         virtual void initiateUndo( PresetController * ) = 0;
         virtual void initiateRedo( PresetController * ) = 0;
   };

   class ParamChange: public ChangeData {
      public:
         Param param;
         float value;

         ParamChange(Param nParam, float nValue)
               :param(nParam),
               value(nValue) {}

         void initiateUndo(PresetController *presetController) override {
            presetController->undoChange(this);
         }

         void initiateRedo(PresetController *presetController) override {
            presetController->redoChange(this);
         }
   };

   class RandomiseChange: public ChangeData {
      public:
         Preset preset;

         RandomiseChange(Preset &nPreset) {
            preset = nPreset; // Uses operator override.
         }

         void initiateUndo(PresetController *presetController) override {
            presetController->undoChange(this);
         }

         void initiateRedo(PresetController *presetController) override {
            presetController->redoChange(this);
         }
   };

   void undoChange	( ParamChange * );
   void undoChange	( RandomiseChange * );
   void redoChange	( ParamChange * );
   void redoChange	( RandomiseChange * );

   std::stack<ChangeData *>	undoBuffer;
   std::stack<ChangeData *>	redoBuffer;
   void clearUndoBuffer	() { while( !undoBuffer.empty() ) { delete undoBuffer.top(); undoBuffer.pop(); } }
   void clearRedoBuffer	() { while( !redoBuffer.empty() ) { delete redoBuffer.top(); redoBuffer.pop(); } }
   void clearChangeBuffers	() { clearUndoBuffer(); clearRedoBuffer(); }

};


*/
