#ifndef DE_APP_WX_HPP
#define DE_APP_WX_HPP

#include "Window_wx.hpp"

// ===================================================================
// App
// ===================================================================
static const wxCmdLineEntryDesc commands [] =
{
   { wxCMD_LINE_SWITCH, "h", "help", "displays help on the command line parameters"},
   { wxCMD_LINE_SWITCH, "i", "input", "input data", wxCMD_LINE_VAL_STRING  },
   { wxCMD_LINE_SWITCH, "o", "output", "output data", wxCMD_LINE_VAL_STRING },
   { wxCMD_LINE_NONE }
};

//static const wxCmdLineEntryDesc commands[] =
//{
//    { wxCMD_LINE_SWITCH, wxT("h"), wxT("help"), wxT("helpmanual") },
////    { wxCMD_LINE_SWITCH, wxT("v"), wxT("verbose"), wxT("be verbose") },
////    { wxCMD_LINE_SWITCH, wxT("q"), wxT("quiet"),   wxT("be quiet") },
//    { wxCMD_LINE_SWITCH, wxT("o"), wxT("output"),  wxT("output file") },
//    { wxCMD_LINE_SWITCH, wxT("i"), wxT("input"),   wxT("input dir") },
////    { wxCMD_LINE_OPTION, wxT("s"), wxT("size"),    wxT("output block size"), wxCMD_LINE_VAL_NUMBER },
////    { wxCMD_LINE_OPTION, wxT("d"), wxT("date"),    wxT("output file date"), wxCMD_LINE_VAL_DATE },
////    { wxCMD_LINE_PARAM,  NULL, NULL, wxT("input file"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_MULTIPLE },
//    { wxCMD_LINE_NONE }
//};

class App : public wxApp
{
    public:
//        virtual void OnInitCmdLine(wxCmdLineParser& parser);
//        virtual bool OnCmdLineParsed(wxCmdLineParser& parser);
        virtual bool OnInit();
        virtual int OnExit();
        virtual int OnRun();
    private:
    //DECLARE_NO_COPY_CLASS(App)
};

DECLARE_APP(App)


#endif
