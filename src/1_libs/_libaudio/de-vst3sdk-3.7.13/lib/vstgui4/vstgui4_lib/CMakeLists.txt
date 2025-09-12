set(LIB_NAME de_vstgui4_lib)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/vstgui/vstgui.h
        ${LIB_HEADER_DIR}/vstgui/lib/animation/animations.h
        ${LIB_HEADER_DIR}/vstgui/lib/animation/animator.h
        ${LIB_HEADER_DIR}/vstgui/lib/animation/ianimationtarget.h
        ${LIB_HEADER_DIR}/vstgui/lib/animation/itimingfunction.h
        ${LIB_HEADER_DIR}/vstgui/lib/animation/timingfunctions.h
		
        ${LIB_HEADER_DIR}/vstgui/lib/algorithm.h		
        ${LIB_HEADER_DIR}/vstgui/lib/cbitmap.h
        ${LIB_HEADER_DIR}/vstgui/lib/cbitmapfilter.h
        ${LIB_HEADER_DIR}/vstgui/lib/cbuttonstate.h
        ${LIB_HEADER_DIR}/vstgui/lib/cclipboard.h
        ${LIB_HEADER_DIR}/vstgui/lib/ccolor.h
        ${LIB_HEADER_DIR}/vstgui/lib/cdatabrowser.h
        ${LIB_HEADER_DIR}/vstgui/lib/cdrawcontext.h
        ${LIB_HEADER_DIR}/vstgui/lib/cdrawdefs.h
        ${LIB_HEADER_DIR}/vstgui/lib/cdrawmethods.h
        ${LIB_HEADER_DIR}/vstgui/lib/cdropsource.h
        ${LIB_HEADER_DIR}/vstgui/lib/cexternalview.h
        ${LIB_HEADER_DIR}/vstgui/lib/cfileselector.h
        ${LIB_HEADER_DIR}/vstgui/lib/cfont.h
        ${LIB_HEADER_DIR}/vstgui/lib/cframe.h
        ${LIB_HEADER_DIR}/vstgui/lib/cgradient.h
        ${LIB_HEADER_DIR}/vstgui/lib/cgradientview.h
        ${LIB_HEADER_DIR}/vstgui/lib/cgraphicspath.h
        ${LIB_HEADER_DIR}/vstgui/lib/cgraphicstransform.h
        ${LIB_HEADER_DIR}/vstgui/lib/cinvalidrectlist.h
        ${LIB_HEADER_DIR}/vstgui/lib/clayeredviewcontainer.h
        ${LIB_HEADER_DIR}/vstgui/lib/clinestyle.h
        ${LIB_HEADER_DIR}/vstgui/lib/coffscreencontext.h
        ${LIB_HEADER_DIR}/vstgui/lib/copenglview.h
        ${LIB_HEADER_DIR}/vstgui/lib/cpoint.h
        ${LIB_HEADER_DIR}/vstgui/lib/crect.h
        ${LIB_HEADER_DIR}/vstgui/lib/cresourcedescription.h
        ${LIB_HEADER_DIR}/vstgui/lib/crowcolumnview.h
        ${LIB_HEADER_DIR}/vstgui/lib/cscrollview.h
        ${LIB_HEADER_DIR}/vstgui/lib/cshadowviewcontainer.h
        ${LIB_HEADER_DIR}/vstgui/lib/csplitview.h
        ${LIB_HEADER_DIR}/vstgui/lib/cstring.h
        ${LIB_HEADER_DIR}/vstgui/lib/ctabview.h
        ${LIB_HEADER_DIR}/vstgui/lib/ctooltipsupport.h
        ${LIB_HEADER_DIR}/vstgui/lib/cview.h
        ${LIB_HEADER_DIR}/vstgui/lib/cviewcontainer.h
        ${LIB_HEADER_DIR}/vstgui/lib/cvstguitimer.h
        ${LIB_HEADER_DIR}/vstgui/lib/dispatchlist.h
        ${LIB_HEADER_DIR}/vstgui/lib/dragging.h
        ${LIB_HEADER_DIR}/vstgui/lib/events.h
        ${LIB_HEADER_DIR}/vstgui/lib/finally.h
        ${LIB_HEADER_DIR}/vstgui/lib/genericstringlistdatabrowsersource.h
        ${LIB_HEADER_DIR}/vstgui/lib/idatabrowserdelegate.h
        ${LIB_HEADER_DIR}/vstgui/lib/idatapackage.h
        ${LIB_HEADER_DIR}/vstgui/lib/idependency.h
        ${LIB_HEADER_DIR}/vstgui/lib/iexternalview.h
        ${LIB_HEADER_DIR}/vstgui/lib/ifocusdrawing.h
        ${LIB_HEADER_DIR}/vstgui/lib/iscalefactorchangedlistener.h
        ${LIB_HEADER_DIR}/vstgui/lib/itouchevent.h
        ${LIB_HEADER_DIR}/vstgui/lib/iviewlistener.h
        ${LIB_HEADER_DIR}/vstgui/lib/malloc.h
        ${LIB_HEADER_DIR}/vstgui/lib/optional.h
        ${LIB_HEADER_DIR}/vstgui/lib/pixelbuffer.h
		${LIB_HEADER_DIR}/vstgui/lib/vstguibase.h
        ${LIB_HEADER_DIR}/vstgui/lib/vstguidebug.h
        ${LIB_HEADER_DIR}/vstgui/lib/vstguifwd.h
        ${LIB_HEADER_DIR}/vstgui/lib/vstguiinit.h
        ${LIB_HEADER_DIR}/vstgui/lib/vstkeycode.h
		
        ${LIB_HEADER_DIR}/vstgui/lib/controls/cautoanimation.h
        ${LIB_HEADER_DIR}/vstgui/lib/controls/cbuttons.h
        ${LIB_HEADER_DIR}/vstgui/lib/controls/ccolorchooser.h
        ${LIB_HEADER_DIR}/vstgui/lib/controls/ccontrol.h
        ${LIB_HEADER_DIR}/vstgui/lib/controls/cfontchooser.h
        ${LIB_HEADER_DIR}/vstgui/lib/controls/cknob.h
        ${LIB_HEADER_DIR}/vstgui/lib/controls/clistcontrol.h
        ${LIB_HEADER_DIR}/vstgui/lib/controls/cmoviebitmap.h
        ${LIB_HEADER_DIR}/vstgui/lib/controls/cmoviebutton.h
        ${LIB_HEADER_DIR}/vstgui/lib/controls/coptionmenu.h
        ${LIB_HEADER_DIR}/vstgui/lib/controls/cparamdisplay.h
        ${LIB_HEADER_DIR}/vstgui/lib/controls/cscrollbar.h
        ${LIB_HEADER_DIR}/vstgui/lib/controls/csearchtextedit.h
        ${LIB_HEADER_DIR}/vstgui/lib/controls/csegmentbutton.h
        ${LIB_HEADER_DIR}/vstgui/lib/controls/cslider.h
        ${LIB_HEADER_DIR}/vstgui/lib/controls/cspecialdigit.h
        ${LIB_HEADER_DIR}/vstgui/lib/controls/csplashscreen.h
        ${LIB_HEADER_DIR}/vstgui/lib/controls/cstringlist.h
        ${LIB_HEADER_DIR}/vstgui/lib/controls/cswitch.h
        ${LIB_HEADER_DIR}/vstgui/lib/controls/ctextedit.h
        ${LIB_HEADER_DIR}/vstgui/lib/controls/ctextlabel.h
        ${LIB_HEADER_DIR}/vstgui/lib/controls/cvumeter.h
        ${LIB_HEADER_DIR}/vstgui/lib/controls/cxypad.h
        ${LIB_HEADER_DIR}/vstgui/lib/controls/icommandmenuitemtarget.h
        ${LIB_HEADER_DIR}/vstgui/lib/controls/icontrollistener.h
        ${LIB_HEADER_DIR}/vstgui/lib/controls/ioptionmenulistener.h
        ${LIB_HEADER_DIR}/vstgui/lib/controls/itexteditlistener.h
        ${LIB_HEADER_DIR}/vstgui/lib/controls/itextlabellistener.h

        ${LIB_HEADER_DIR}/vstgui/lib/platform/common/fileresourceinputstream.h
        ${LIB_HEADER_DIR}/vstgui/lib/platform/common/genericoptionmenu.h
        ${LIB_HEADER_DIR}/vstgui/lib/platform/common/generictextedit.h
        ${LIB_HEADER_DIR}/vstgui/lib/platform/common/gradientbase.h
        ${LIB_HEADER_DIR}/vstgui/lib/platform/common/stb_textedit.h		
        ${LIB_HEADER_DIR}/vstgui/lib/platform/iplatformbitmap.h
        ${LIB_HEADER_DIR}/vstgui/lib/platform/iplatformfileselector.h
        ${LIB_HEADER_DIR}/vstgui/lib/platform/iplatformfont.h
        ${LIB_HEADER_DIR}/vstgui/lib/platform/iplatformframe.h
        ${LIB_HEADER_DIR}/vstgui/lib/platform/iplatformframecallback.h
        ${LIB_HEADER_DIR}/vstgui/lib/platform/iplatformgradient.h
        ${LIB_HEADER_DIR}/vstgui/lib/platform/iplatformgraphicsdevice.h
        ${LIB_HEADER_DIR}/vstgui/lib/platform/iplatformgraphicspath.h
        ${LIB_HEADER_DIR}/vstgui/lib/platform/iplatformopenglview.h
        ${LIB_HEADER_DIR}/vstgui/lib/platform/iplatformoptionmenu.h
        ${LIB_HEADER_DIR}/vstgui/lib/platform/iplatformresourceinputstream.h
        ${LIB_HEADER_DIR}/vstgui/lib/platform/iplatformstring.h
        ${LIB_HEADER_DIR}/vstgui/lib/platform/iplatformtextedit.h
        ${LIB_HEADER_DIR}/vstgui/lib/platform/iplatformtimer.h
        ${LIB_HEADER_DIR}/vstgui/lib/platform/iplatformviewlayer.h
        ${LIB_HEADER_DIR}/vstgui/lib/platform/platformfactory.h
        ${LIB_HEADER_DIR}/vstgui/lib/platform/platformfwd.h
        ${LIB_HEADER_DIR}/vstgui/lib/platform/platform_macos.h
        ${LIB_HEADER_DIR}/vstgui/lib/platform/platform_win32.h
        ${LIB_HEADER_DIR}/vstgui/lib/platform/platform_x11.h
        ${LIB_HEADER_DIR}/vstgui/lib/platform/std_unorderedmap.h
		
        # ${LIB_HEADER_DIR}/vstgui/lib/platform/linux/cairobitmap.h
        # ${LIB_HEADER_DIR}/vstgui/lib/platform/linux/cairofont.h
        # ${LIB_HEADER_DIR}/vstgui/lib/platform/linux/cairogradient.h
        # ${LIB_HEADER_DIR}/vstgui/lib/platform/linux/cairographicscontext.h
        # ${LIB_HEADER_DIR}/vstgui/lib/platform/linux/cairopath.h
        # ${LIB_HEADER_DIR}/vstgui/lib/platform/linux/cairoutils.h
        # ${LIB_HEADER_DIR}/vstgui/lib/platform/linux/irunloop.h
        # ${LIB_HEADER_DIR}/vstgui/lib/platform/linux/linuxfactory.h
        # ${LIB_HEADER_DIR}/vstgui/lib/platform/linux/linuxstring.h
        # ${LIB_HEADER_DIR}/vstgui/lib/platform/linux/x11dragging.h
        # ${LIB_HEADER_DIR}/vstgui/lib/platform/linux/x11fileselector.h
        # ${LIB_HEADER_DIR}/vstgui/lib/platform/linux/x11frame.h
        # ${LIB_HEADER_DIR}/vstgui/lib/platform/linux/x11platform.h
        # ${LIB_HEADER_DIR}/vstgui/lib/platform/linux/x11timer.h
        # ${LIB_HEADER_DIR}/vstgui/lib/platform/linux/x11utils.h

        # ${LIB_HEADER_DIR}/vstgui/lib/platform/mac/caviewlayer.h
        # ${LIB_HEADER_DIR}/vstgui/lib/platform/mac/cfontmac.h
        # ${LIB_HEADER_DIR}/vstgui/lib/platform/mac/cgbitmap.h
        # ${LIB_HEADER_DIR}/vstgui/lib/platform/mac/cocoa/autoreleasepool.h
        # ${LIB_HEADER_DIR}/vstgui/lib/platform/mac/cocoa/cocoahelpers.h
        # ${LIB_HEADER_DIR}/vstgui/lib/platform/mac/cocoa/cocoaopenglview.h
        # ${LIB_HEADER_DIR}/vstgui/lib/platform/mac/cocoa/cocoatextedit.h
        # ${LIB_HEADER_DIR}/vstgui/lib/platform/mac/cocoa/nsviewdraggingsession.h
        # ${LIB_HEADER_DIR}/vstgui/lib/platform/mac/cocoa/nsviewframe.h
        # ${LIB_HEADER_DIR}/vstgui/lib/platform/mac/cocoa/nsviewoptionmenu.h
        # ${LIB_HEADER_DIR}/vstgui/lib/platform/mac/cocoa/objcclassbuilder.h
        # ${LIB_HEADER_DIR}/vstgui/lib/platform/mac/coregraphicsdevicecontext.h
        # ${LIB_HEADER_DIR}/vstgui/lib/platform/mac/ios/uiopenglview.h
        # ${LIB_HEADER_DIR}/vstgui/lib/platform/mac/ios/uitextedit.h
        # ${LIB_HEADER_DIR}/vstgui/lib/platform/mac/ios/uitouchevent.h
        # ${LIB_HEADER_DIR}/vstgui/lib/platform/mac/ios/uiviewframe.h
        # ${LIB_HEADER_DIR}/vstgui/lib/platform/mac/macclipboard.h
        # ${LIB_HEADER_DIR}/vstgui/lib/platform/mac/macfactory.h
        # ${LIB_HEADER_DIR}/vstgui/lib/platform/mac/macfileselector.h
        # ${LIB_HEADER_DIR}/vstgui/lib/platform/mac/macglobals.h
        # ${LIB_HEADER_DIR}/vstgui/lib/platform/mac/macstring.h
        # ${LIB_HEADER_DIR}/vstgui/lib/platform/mac/mactimer.h
        # ${LIB_HEADER_DIR}/vstgui/lib/platform/mac/quartzgraphicspath.h

        ${LIB_HEADER_DIR}/vstgui/lib/platform/win32/direct2d/d2d.h
        ${LIB_HEADER_DIR}/vstgui/lib/platform/win32/direct2d/d2dbitmap.h
        ${LIB_HEADER_DIR}/vstgui/lib/platform/win32/direct2d/d2dbitmapcache.h
        ${LIB_HEADER_DIR}/vstgui/lib/platform/win32/direct2d/d2dfont.h
        ${LIB_HEADER_DIR}/vstgui/lib/platform/win32/direct2d/d2dgradient.h
        ${LIB_HEADER_DIR}/vstgui/lib/platform/win32/direct2d/d2dgraphicscontext.h
        ${LIB_HEADER_DIR}/vstgui/lib/platform/win32/direct2d/d2dgraphicspath.h
		${LIB_HEADER_DIR}/vstgui/lib/platform/win32/comptr.h
        ${LIB_HEADER_DIR}/vstgui/lib/platform/win32/win32bitmapbase.h
        ${LIB_HEADER_DIR}/vstgui/lib/platform/win32/win32datapackage.h
        ${LIB_HEADER_DIR}/vstgui/lib/platform/win32/win32directcomposition.h
        ${LIB_HEADER_DIR}/vstgui/lib/platform/win32/win32dll.h
        ${LIB_HEADER_DIR}/vstgui/lib/platform/win32/win32dragging.h
        ${LIB_HEADER_DIR}/vstgui/lib/platform/win32/win32factory.h
        ${LIB_HEADER_DIR}/vstgui/lib/platform/win32/win32frame.h
        ${LIB_HEADER_DIR}/vstgui/lib/platform/win32/win32openglview.h
        ${LIB_HEADER_DIR}/vstgui/lib/platform/win32/win32optionmenu.h
        ${LIB_HEADER_DIR}/vstgui/lib/platform/win32/win32resourcestream.h
        ${LIB_HEADER_DIR}/vstgui/lib/platform/win32/win32support.h
        ${LIB_HEADER_DIR}/vstgui/lib/platform/win32/win32textedit.h
        ${LIB_HEADER_DIR}/vstgui/lib/platform/win32/win32viewlayer.h
        ${LIB_HEADER_DIR}/vstgui/lib/platform/win32/winfileselector.h
        ${LIB_HEADER_DIR}/vstgui/lib/platform/win32/winstring.h
        ${LIB_HEADER_DIR}/vstgui/lib/platform/win32/wintimer.h

        ${LIB_HEADER_DIR}/vstgui/lib/private/disabledeprecatedmessage.h
        ${LIB_HEADER_DIR}/vstgui/lib/private/enabledeprecatedmessage.h
		
        ${LIB_HEADER_DIR}/vstgui/uidescription/base64codec.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/compresseduidescription.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/cstream.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/delegationcontroller.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/doc.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/icontentprovider.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/icontroller.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/iuidescription.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/iviewcreator.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/iviewfactory.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/uiattributes.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/uicontentprovider.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/uidescription.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/uidescriptionfwd.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/uidescriptionlistener.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/uiviewcreator.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/uiviewfactory.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/uiviewswitchcontainer.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/xmlparser.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/xmluidescdoc.h
		
        # ${LIB_HEADER_DIR}/vstgui/uidescription/detail/locale.h   # bad
        ${LIB_HEADER_DIR}/vstgui/uidescription/detail/parsecolor.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/detail/scalefactorutils.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/detail/uidesclist.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/detail/uijsonpersistence.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/detail/uinode.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/detail/uiviewcreatorattributes.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/detail/uixmlpersistence.h

        ${LIB_HEADER_DIR}/vstgui/uidescription/editing/createuidescdata.sh
        ${LIB_HEADER_DIR}/vstgui/uidescription/editing/doc.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/editing/iaction.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/editing/igridprocessor.h

        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/attach-bottom#2.0x.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/attach-bottom-non#2.0x.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/attach-bottom-non.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/attach-bottom.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/attach-left#2.0x.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/attach-left-non#2.0x.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/attach-left-non.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/attach-left.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/attach-right#2.0x.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/attach-right-non#2.0x.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/attach-right-non.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/attach-right.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/attach-top#2.0x.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/attach-top-non#2.0x.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/attach-top-non.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/attach-top.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/darkmode-attach-bottom#2.0x.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/darkmode-attach-bottom-non#2.0x.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/darkmode-attach-bottom-non.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/darkmode-attach-bottom.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/darkmode-attach-left#2.0x.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/darkmode-attach-left-non#2.0x.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/darkmode-attach-left-non.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/darkmode-attach-left.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/darkmode-attach-right#2.0x.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/darkmode-attach-right-non#2.0x.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/darkmode-attach-right-non.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/darkmode-attach-right.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/darkmode-attach-top#2.0x.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/darkmode-attach-top-non#2.0x.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/darkmode-attach-top-non.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/darkmode-attach-top.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/darkmode-segment-fonts#2.0x.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/darkmode-segment-fonts.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/darkmode-segment-tags#2.0x.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/darkmode-segment-tags.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/darkmode-segment-views#2.0x.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/darkmode-segment-views.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/segment-bitmaps#2.0x.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/segment-bitmaps.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/segment-colors#2.0x.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/segment-colors.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/segment-fonts#2.0x.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/segment-fonts.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/segment-gradients#2.0x.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/segment-gradients.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/segment-tags#2.0x.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/segment-tags.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/segment-views#2.0x.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/segment-views.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/text-align-center-white.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/text-align-center.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/text-align-left-white.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/text-align-left.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/text-align-right-white.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/text-align-right.png

        ${LIB_HEADER_DIR}/vstgui/uidescription/editing/uiactions.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/editing/uiattributescontroller.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/editing/uibasedatasource.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/editing/uibitmapscontroller.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/editing/uicolor.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/editing/uicolorchoosercontroller.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/editing/uicolorscontroller.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/editing/uicolorslider.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/editing/uicrosslines.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/editing/uidescriptioneditor.uidesc
        ${LIB_HEADER_DIR}/vstgui/uidescription/editing/uidescriptioneditor_res_dark.uidesc
        ${LIB_HEADER_DIR}/vstgui/uidescription/editing/uidescriptioneditor_res_light.uidesc
        ${LIB_HEADER_DIR}/vstgui/uidescription/editing/uidialogcontroller.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/editing/uieditcontroller.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/editing/uieditmenucontroller.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/editing/uieditview.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/editing/uifocussettingscontroller.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/editing/uifontscontroller.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/editing/uigradientscontroller.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/editing/uigrid.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/editing/uigridcontroller.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/editing/uioverlayview.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/editing/uiselection.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/editing/uitagscontroller.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/editing/uitemplatecontroller.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/editing/uitemplatesettingscontroller.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/editing/uiundomanager.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/editing/uiviewcreatecontroller.h

        ${LIB_HEADER_DIR}/vstgui/uidescription/viewcreator/animationsplashscreencreator.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/viewcreator/animknobcreator.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/viewcreator/autoanimationcreator.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/viewcreator/checkboxcreator.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/viewcreator/controlcreator.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/viewcreator/gradientviewcreator.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/viewcreator/kickbuttoncreator.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/viewcreator/knobcreator.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/viewcreator/layeredviewcontainercreator.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/viewcreator/moviebitmapcreator.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/viewcreator/moviebuttoncreator.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/viewcreator/multibitmapcontrolcreator.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/viewcreator/multilinetextlabelcreator.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/viewcreator/onoffbuttoncreator.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/viewcreator/optionmenucreator.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/viewcreator/paramdisplaycreator.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/viewcreator/rockerswitchcreator.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/viewcreator/rowcolumnviewcreator.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/viewcreator/scrollviewcreator.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/viewcreator/searchtexteditcreator.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/viewcreator/segmentbuttoncreator.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/viewcreator/shadowviewcontainercreator.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/viewcreator/sliderviewcreator.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/viewcreator/splitviewcreator.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/viewcreator/stringlistcontrolcreator.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/viewcreator/switchcreators.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/viewcreator/textbuttoncreator.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/viewcreator/texteditcreator.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/viewcreator/textlabelcreator.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/viewcreator/uiviewswitchcontainercreator.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/viewcreator/viewcontainercreator.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/viewcreator/viewcreator.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/viewcreator/vumetercreator.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/viewcreator/xypadcreator.h
		
    )	
	
    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/vstgui/lib/animation/animations.cpp
        ${LIB_SOURCE_DIR}/vstgui/lib/animation/animator.cpp
        ${LIB_SOURCE_DIR}/vstgui/lib/animation/timingfunctions.cpp

        ${LIB_SOURCE_DIR}/vstgui/lib/cbitmap.cpp
        ${LIB_SOURCE_DIR}/vstgui/lib/cbitmapfilter.cpp
        ${LIB_SOURCE_DIR}/vstgui/lib/cclipboard.cpp
        ${LIB_SOURCE_DIR}/vstgui/lib/ccolor.cpp
        ${LIB_SOURCE_DIR}/vstgui/lib/cdatabrowser.cpp
        ${LIB_SOURCE_DIR}/vstgui/lib/cdrawcontext.cpp
        ${LIB_SOURCE_DIR}/vstgui/lib/cdrawmethods.cpp
        ${LIB_SOURCE_DIR}/vstgui/lib/cdropsource.cpp
        ${LIB_SOURCE_DIR}/vstgui/lib/cexternalview.cpp
        ${LIB_SOURCE_DIR}/vstgui/lib/cfileselector.cpp
        ${LIB_SOURCE_DIR}/vstgui/lib/cfont.cpp
        ${LIB_SOURCE_DIR}/vstgui/lib/cframe.cpp
        ${LIB_SOURCE_DIR}/vstgui/lib/cgradient.cpp
        ${LIB_SOURCE_DIR}/vstgui/lib/cgradientview.cpp
        ${LIB_SOURCE_DIR}/vstgui/lib/cgraphicspath.cpp
        ${LIB_SOURCE_DIR}/vstgui/lib/clayeredviewcontainer.cpp
        ${LIB_SOURCE_DIR}/vstgui/lib/clinestyle.cpp
        ${LIB_SOURCE_DIR}/vstgui/lib/coffscreencontext.cpp
        ${LIB_SOURCE_DIR}/vstgui/lib/copenglview.cpp
        ${LIB_SOURCE_DIR}/vstgui/lib/cpoint.cpp
        ${LIB_SOURCE_DIR}/vstgui/lib/crect.cpp
        ${LIB_SOURCE_DIR}/vstgui/lib/crowcolumnview.cpp
        ${LIB_SOURCE_DIR}/vstgui/lib/cscrollview.cpp
        ${LIB_SOURCE_DIR}/vstgui/lib/cshadowviewcontainer.cpp
        ${LIB_SOURCE_DIR}/vstgui/lib/csplitview.cpp
        ${LIB_SOURCE_DIR}/vstgui/lib/cstring.cpp
        ${LIB_SOURCE_DIR}/vstgui/lib/ctabview.cpp
        ${LIB_SOURCE_DIR}/vstgui/lib/ctooltipsupport.cpp
        ${LIB_SOURCE_DIR}/vstgui/lib/cview.cpp
        ${LIB_SOURCE_DIR}/vstgui/lib/cviewcontainer.cpp
        ${LIB_SOURCE_DIR}/vstgui/lib/cvstguitimer.cpp
        ${LIB_SOURCE_DIR}/vstgui/lib/events.cpp
        ${LIB_SOURCE_DIR}/vstgui/lib/genericstringlistdatabrowsersource.cpp
        ${LIB_SOURCE_DIR}/vstgui/lib/pixelbuffer.cpp
        ${LIB_SOURCE_DIR}/vstgui/lib/vstguidebug.cpp
        ${LIB_SOURCE_DIR}/vstgui/lib/vstguiinit.cpp
		
        ${LIB_SOURCE_DIR}/vstgui/lib/controls/cautoanimation.cpp
        ${LIB_SOURCE_DIR}/vstgui/lib/controls/cbuttons.cpp
        ${LIB_SOURCE_DIR}/vstgui/lib/controls/ccolorchooser.cpp
        ${LIB_SOURCE_DIR}/vstgui/lib/controls/ccontrol.cpp
        ${LIB_SOURCE_DIR}/vstgui/lib/controls/cfontchooser.cpp
        ${LIB_SOURCE_DIR}/vstgui/lib/controls/cknob.cpp
        ${LIB_SOURCE_DIR}/vstgui/lib/controls/clistcontrol.cpp
        ${LIB_SOURCE_DIR}/vstgui/lib/controls/cmoviebitmap.cpp
        ${LIB_SOURCE_DIR}/vstgui/lib/controls/cmoviebutton.cpp
        ${LIB_SOURCE_DIR}/vstgui/lib/controls/coptionmenu.cpp
        ${LIB_SOURCE_DIR}/vstgui/lib/controls/cparamdisplay.cpp
        ${LIB_SOURCE_DIR}/vstgui/lib/controls/cscrollbar.cpp
        ${LIB_SOURCE_DIR}/vstgui/lib/controls/csearchtextedit.cpp
        ${LIB_SOURCE_DIR}/vstgui/lib/controls/csegmentbutton.cpp
        ${LIB_SOURCE_DIR}/vstgui/lib/controls/cslider.cpp
        ${LIB_SOURCE_DIR}/vstgui/lib/controls/cspecialdigit.cpp
        ${LIB_SOURCE_DIR}/vstgui/lib/controls/csplashscreen.cpp
        ${LIB_SOURCE_DIR}/vstgui/lib/controls/cstringlist.cpp
        ${LIB_SOURCE_DIR}/vstgui/lib/controls/cswitch.cpp
        ${LIB_SOURCE_DIR}/vstgui/lib/controls/ctextedit.cpp
        ${LIB_SOURCE_DIR}/vstgui/lib/controls/ctextlabel.cpp
        ${LIB_SOURCE_DIR}/vstgui/lib/controls/cvumeter.cpp
        ${LIB_SOURCE_DIR}/vstgui/lib/controls/cxypad.cpp

        ${LIB_SOURCE_DIR}/vstgui/lib/platform/common/fileresourceinputstream.cpp
        ${LIB_SOURCE_DIR}/vstgui/lib/platform/common/genericoptionmenu.cpp
        ${LIB_SOURCE_DIR}/vstgui/lib/platform/common/generictextedit.cpp
        ${LIB_SOURCE_DIR}/vstgui/lib/platform/platformfactory.cpp
		
        # ${LIB_SOURCE_DIR}/vstgui/lib/platform/linux/cairobitmap.cpp
        # ${LIB_SOURCE_DIR}/vstgui/lib/platform/linux/cairofont.cpp
        # ${LIB_SOURCE_DIR}/vstgui/lib/platform/linux/cairogradient.cpp
        # ${LIB_SOURCE_DIR}/vstgui/lib/platform/linux/cairographicscontext.cpp
        # ${LIB_SOURCE_DIR}/vstgui/lib/platform/linux/cairopath.cpp
        # ${LIB_SOURCE_DIR}/vstgui/lib/platform/linux/linuxfactory.cpp
        # ${LIB_SOURCE_DIR}/vstgui/lib/platform/linux/linuxstring.cpp
        # ${LIB_SOURCE_DIR}/vstgui/lib/platform/linux/x11dragging.cpp
        # ${LIB_SOURCE_DIR}/vstgui/lib/platform/linux/x11fileselector.cpp
        # ${LIB_SOURCE_DIR}/vstgui/lib/platform/linux/x11frame.cpp
        # ${LIB_SOURCE_DIR}/vstgui/lib/platform/linux/x11platform.cpp
        # ${LIB_SOURCE_DIR}/vstgui/lib/platform/linux/x11timer.cpp
        # ${LIB_SOURCE_DIR}/vstgui/lib/platform/linux/x11utils.cpp

        # ${LIB_SOURCE_DIR}/vstgui/lib/platform/mac/caviewlayer.mm
        # ${LIB_SOURCE_DIR}/vstgui/lib/platform/mac/cfontmac.mm
        # ${LIB_SOURCE_DIR}/vstgui/lib/platform/mac/cgbitmap.cpp

        # ${LIB_SOURCE_DIR}/vstgui/lib/platform/mac/cocoa/autoreleasepool.mm
        # ${LIB_SOURCE_DIR}/vstgui/lib/platform/mac/cocoa/cocoahelpers.mm
        # ${LIB_SOURCE_DIR}/vstgui/lib/platform/mac/cocoa/cocoaopenglview.mm
        # ${LIB_SOURCE_DIR}/vstgui/lib/platform/mac/cocoa/cocoatextedit.mm
        # ${LIB_SOURCE_DIR}/vstgui/lib/platform/mac/cocoa/nsviewdraggingsession.mm
        # ${LIB_SOURCE_DIR}/vstgui/lib/platform/mac/cocoa/nsviewframe.mm
        # ${LIB_SOURCE_DIR}/vstgui/lib/platform/mac/cocoa/nsviewoptionmenu.mm
        # ${LIB_SOURCE_DIR}/vstgui/lib/platform/mac/coregraphicsdevicecontext.mm

        # ${LIB_SOURCE_DIR}/vstgui/lib/platform/mac/ios/uiopenglview.mm
        # ${LIB_SOURCE_DIR}/vstgui/lib/platform/mac/ios/uitextedit.mm
        # ${LIB_SOURCE_DIR}/vstgui/lib/platform/mac/ios/uiviewframe.mm
        # ${LIB_SOURCE_DIR}/vstgui/lib/platform/mac/macclipboard.mm
        # ${LIB_SOURCE_DIR}/vstgui/lib/platform/mac/macfactory.mm
        # ${LIB_SOURCE_DIR}/vstgui/lib/platform/mac/macfileselector.mm
        # ${LIB_SOURCE_DIR}/vstgui/lib/platform/mac/macglobals.cpp
        # ${LIB_SOURCE_DIR}/vstgui/lib/platform/mac/macstring.mm
        # ${LIB_SOURCE_DIR}/vstgui/lib/platform/mac/mactimer.cpp
        # ${LIB_SOURCE_DIR}/vstgui/lib/platform/mac/quartzgraphicspath.cpp

        ${LIB_SOURCE_DIR}/vstgui/lib/platform/win32/direct2d/d2dbitmap.cpp
        ${LIB_SOURCE_DIR}/vstgui/lib/platform/win32/direct2d/d2dbitmapcache.cpp
        ${LIB_SOURCE_DIR}/vstgui/lib/platform/win32/direct2d/d2dfont.cpp
        ${LIB_SOURCE_DIR}/vstgui/lib/platform/win32/direct2d/d2dgradient.cpp
        ${LIB_SOURCE_DIR}/vstgui/lib/platform/win32/direct2d/d2dgraphicscontext.cpp
        ${LIB_SOURCE_DIR}/vstgui/lib/platform/win32/direct2d/d2dgraphicspath.cpp
        ${LIB_SOURCE_DIR}/vstgui/lib/platform/win32/win32datapackage.cpp
        ${LIB_SOURCE_DIR}/vstgui/lib/platform/win32/win32directcomposition.cpp
        ${LIB_SOURCE_DIR}/vstgui/lib/platform/win32/win32dragging.cpp
        ${LIB_SOURCE_DIR}/vstgui/lib/platform/win32/win32factory.cpp
        ${LIB_SOURCE_DIR}/vstgui/lib/platform/win32/win32frame.cpp
        ${LIB_SOURCE_DIR}/vstgui/lib/platform/win32/win32openglview.cpp
        ${LIB_SOURCE_DIR}/vstgui/lib/platform/win32/win32optionmenu.cpp
        ${LIB_SOURCE_DIR}/vstgui/lib/platform/win32/win32resourcestream.cpp
        ${LIB_SOURCE_DIR}/vstgui/lib/platform/win32/win32support.cpp
        ${LIB_SOURCE_DIR}/vstgui/lib/platform/win32/win32textedit.cpp
        ${LIB_SOURCE_DIR}/vstgui/lib/platform/win32/win32viewlayer.cpp
        ${LIB_SOURCE_DIR}/vstgui/lib/platform/win32/winfileselector.cpp
        ${LIB_SOURCE_DIR}/vstgui/lib/platform/win32/winstring.cpp
        ${LIB_SOURCE_DIR}/vstgui/lib/platform/win32/wintimer.cpp

        ${LIB_SOURCE_DIR}/vstgui/lib/private/disabledeprecatedmessage.h
        ${LIB_SOURCE_DIR}/vstgui/lib/private/enabledeprecatedmessage.h		

        ${LIB_SOURCE_DIR}/vstgui/uidescription/compresseduidescription.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/cstream.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/uiattributes.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/uicontentprovider.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/uidescription.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/uiviewcreator.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/uiviewfactory.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/uiviewswitchcontainer.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/xmlparser.cpp
		
        ${LIB_SOURCE_DIR}/vstgui/uidescription/detail/uidesclist.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/detail/uijsonpersistence.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/detail/uinode.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/detail/uixmlpersistence.cpp

        ${LIB_SOURCE_DIR}/vstgui/uidescription/editing/uiactions.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/editing/uiattributescontroller.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/editing/uibitmapscontroller.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/editing/uicolor.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/editing/uicolorchoosercontroller.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/editing/uicolorscontroller.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/editing/uicolorslider.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/editing/uicrosslines.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/editing/uidialogcontroller.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/editing/uieditcontroller.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/editing/uieditmenucontroller.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/editing/uieditview.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/editing/uifocussettingscontroller.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/editing/uifontscontroller.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/editing/uigradientscontroller.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/editing/uigridcontroller.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/editing/uioverlayview.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/editing/uiselection.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/editing/uitagscontroller.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/editing/uitemplatecontroller.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/editing/uitemplatesettingscontroller.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/editing/uiundomanager.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/editing/uiviewcreatecontroller.cpp

        ${LIB_SOURCE_DIR}/vstgui/uidescription/viewcreator/animationsplashscreencreator.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/viewcreator/animknobcreator.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/viewcreator/autoanimationcreator.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/viewcreator/checkboxcreator.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/viewcreator/controlcreator.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/viewcreator/gradientviewcreator.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/viewcreator/kickbuttoncreator.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/viewcreator/knobcreator.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/viewcreator/layeredviewcontainercreator.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/viewcreator/moviebitmapcreator.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/viewcreator/moviebuttoncreator.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/viewcreator/multibitmapcontrolcreator.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/viewcreator/multilinetextlabelcreator.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/viewcreator/onoffbuttoncreator.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/viewcreator/optionmenucreator.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/viewcreator/paramdisplaycreator.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/viewcreator/rockerswitchcreator.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/viewcreator/rowcolumnviewcreator.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/viewcreator/scrollviewcreator.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/viewcreator/searchtexteditcreator.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/viewcreator/segmentbuttoncreator.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/viewcreator/shadowviewcontainercreator.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/viewcreator/sliderviewcreator.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/viewcreator/splitviewcreator.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/viewcreator/stringlistcontrolcreator.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/viewcreator/switchcreators.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/viewcreator/textbuttoncreator.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/viewcreator/texteditcreator.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/viewcreator/textlabelcreator.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/viewcreator/uiviewswitchcontainercreator.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/viewcreator/viewcontainercreator.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/viewcreator/viewcreator.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/viewcreator/vumetercreator.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/viewcreator/xypadcreator.cpp

    )
	
	# if(WIN32)
        # enable_language(RC)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    # endif()

    if (BUILD_SHARED)
        message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} SHARED "")
    else()
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")	
        add_library(${LIB_NAME} STATIC "")
    endif()

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR}/vstgui/lib)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR}/vstgui/lib/animation)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR}/vstgui/lib/controls)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR}/vstgui/lib/platform)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR}/vstgui/lib/platform/common)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR}/vstgui/lib/platform/win32)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR}/vstgui/lib/platform/win32/direct2d)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR}/vstgui/uidescription)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR}/vstgui/uidescription/detail)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR}/vstgui/uidescription/editing)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR}/vstgui/uidescription/viewcreator)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/vstgui/lib)

    target_compile_definitions(${LIB_NAME} PRIVATE VSTGUI_USE_SYSTEM_EXPAT=1)
    # "$<$<CONFIG:Debug>:-DDEVELOPMENT=1>"
    # "$<$<CONFIG:Release>:-DRELEASE=1>"
    # "$<$<CONFIG:RelWithDebInfo>:-DRELEASE=1>"
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_definitions(${LIB_NAME} PRIVATE DEVELOPMENT=1)
    else()
        target_compile_definitions(${LIB_NAME} PRIVATE RELEASE=1)
    endif()

    target_link_libraries(${LIB_NAME} PUBLIC de_minizip)
    target_link_libraries(${LIB_NAME} PUBLIC de_expat)
    target_link_libraries(${LIB_NAME} PUBLIC de_rapidjson)

    if (WIN32)
        target_link_libraries(${LIB_NAME} PRIVATE shlwapi d3d11 dwmapi d2d1 dwrite opengl32 )
    endif()

    if (MSVC)
	else()
        # target_compile_options(${LIB_NAME} PUBLIC -Wno-deprecated-declarations)
        target_compile_options(${LIB_NAME} PUBLIC -Wno-multichar)
    endif()

endif()


