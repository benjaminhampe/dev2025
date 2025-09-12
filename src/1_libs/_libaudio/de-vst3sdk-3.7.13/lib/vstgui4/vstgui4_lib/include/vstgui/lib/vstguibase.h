// This file is part of VSTGUI. It is subject to the license terms
// in the LICENSE file found in the top-level directory of this
// distribution and at http://github.com/steinbergmedia/vstgui/LICENSE

#pragma once

#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <utility>
#include <memory>
#include <limits>
#include <vector>
#include <map>
#include <locale>
//#include <sstream>

#include <string>
#include <functional>
//#include <algorithm>
#include "vstgui/lib/optional.h"

//-----------------------------------------------------------------------------
// VSTGUI Version
//-----------------------------------------------------------------------------
#define VSTGUI_VERSION_MAJOR  4
#define VSTGUI_VERSION_MINOR  14
#define VSTGUI_VERSION_PATCHLEVEL  2

//-----------------------------------------------------------------------------
// Platform definitions
//-----------------------------------------------------------------------------
#if __APPLE_CC__
	#ifndef __ASSERT_MACROS_DEFINE_VERSIONS_WITHOUT_UNDERSCORES
		#define __ASSERT_MACROS_DEFINE_VERSIONS_WITHOUT_UNDERSCORES 0
	#endif
	#include <stdint.h>
	#include <AvailabilityMacros.h>
	#include <TargetConditionals.h>
	#if TARGET_OS_IPHONE
		#ifndef MAC
			#define MAC 1
		#endif
		#ifndef MAC_COCOA
			#define MAC_COCOA 1
		#endif
		#define VSTGUI_OPENGL_SUPPORT 0	// there's an implementation, but not yet tested, so this is zero
		#define VSTGUI_TOUCH_EVENT_HANDLING 1
	#else
		#ifndef MAC_OS_X_VERSION_10_9
			#error you need at least OSX SDK 10.9 to build vstgui
		#endif
		#ifndef MAC_COCOA
			#define MAC_COCOA 1
		#endif
		#ifndef MAC
			#define MAC 1
		#endif
	#endif

	#ifndef __has_feature
		#error compiler not supported
	#endif
	#if __has_feature (cxx_rvalue_references) == 0
		#error need cxx_rvalue_references support from compiler
	#endif
	#if __has_feature (cxx_range_for) == 0
		#error need cxx_range_for support from compiler
	#endif
	#include <type_traits>

	#if defined (__clang__) && __clang_major__ > 4
		#if defined (VSTGUI_WARN_EVERYTHING) && VSTGUI_WARN_EVERYTHING == 1
			#pragma clang diagnostic warning "-Weverything"
			#pragma clang diagnostic warning "-Wconversion"
			#pragma clang diagnostic ignored "-Wreorder"
		#else
			#pragma clang diagnostic warning "-Wunreachable-code"
			#if __clang_major__ > 7
				#pragma clang diagnostic warning "-Wshadow"
			#endif
		#endif
	#endif

#elif WIN32 || WINDOWS || defined(_WIN32)
	#ifndef NOMINMAX
		#define NOMINMAX
	#endif
	#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN
	#endif

	#include <sdkddkver.h>
	#if _WIN32_WINNT < 0x601
		#error unsupported Platform SDK you need at least the Windows 7 Platform SDK to compile VSTGUI
	#endif
	#ifdef __GNUC__
		#if __cplusplus < 201103L
			#error compiler not supported
		#endif
	#elif _MSC_VER <= 1800
		#error Visual Studio 2015 or newer needed
	#endif
	#include <type_traits>
	#include <cstdint>
	#ifndef WINDOWS
		#define WINDOWS 1
	#endif
	#if (defined(_M_ARM64) || defined(_M_ARM))
		#if defined(VSTGUI_OPENGL_SUPPORT)
			#undef VSTGUI_OPENGL_SUPPORT
		#endif
		#define VSTGUI_OPENGL_SUPPORT 0
	#endif
	#ifdef _MSC_VER
		#pragma warning(3 : 4189) // local variable is initialized but not referenced
		#pragma warning(3 : 4702) // unreachable code
		#pragma warning(3 : 4995) // deprecated
		#pragma warning(3 : 4431) // missing type specifier - int assumed. Note: C no longer supports default-int
		#pragma warning(3 : 4254) // conversion from 'type1' to 'type2', possible loss of data
		#pragma warning(3 : 4388) // signed/unsigned mismatch
		#pragma warning(disable : 4250) // class1' : inherits 'class2::member' via dominance
	#endif

	#if defined (__clang__) && __clang__
		#if defined (VSTGUI_WARN_EVERYTHING) && VSTGUI_WARN_EVERYTHING == 1
			#pragma clang diagnostic warning "-Wconversion"
			#pragma clang diagnostic ignored "-Wreorder"
		#else
			#pragma clang diagnostic warning "-Wunreachable-code"
		#endif
	#endif

    //using std::min;
    //using std::max;

#elif defined(__linux__)
    #include <cstdint>
    #include <type_traits>
    #include <algorithm>
    #include <climits>
    using std::min;
    using std::max;
	#ifndef LINUX
		#define LINUX 1
	#endif

#else
#error unsupported system/compiler
#endif

#include <atomic>
#include <utility>

#ifdef UNICODE
	#undef UNICODE
#endif
#define UNICODE 1

//----------------------------------------------------
// Deprecation setting
//----------------------------------------------------
#ifndef VSTGUI_ENABLE_DEPRECATED_METHODS
	#define VSTGUI_ENABLE_DEPRECATED_METHODS 1
#endif

#if VSTGUI_ENABLE_DEPRECATED_METHODS
	#define VSTGUI_DEPRECATED(x)			[[deprecated]]	x
	#define VSTGUI_DEPRECATED_MSG(x, msg)	[[deprecated(msg)]]	x
#else
	#define VSTGUI_DEPRECATED(x)
	#define VSTGUI_DEPRECATED_MSG(x, msg)
#endif

//----------------------------------------------------
// Feature setting
//----------------------------------------------------
#ifndef VSTGUI_OPENGL_SUPPORT
	#define VSTGUI_OPENGL_SUPPORT 1
#endif

#ifndef VSTGUI_TOUCH_EVENT_HANDLING
	#define VSTGUI_TOUCH_EVENT_HANDLING 0
#endif

#ifndef VSTGUI_ENABLE_XML_PARSER
	#define VSTGUI_ENABLE_XML_PARSER 1
#endif

#if VSTGUI_ENABLE_DEPRECATED_METHODS
	#define VSTGUI_OVERRIDE_VMETHOD	override
	#define VSTGUI_FINAL_VMETHOD final
#else
	#define VSTGUI_OVERRIDE_VMETHOD	static_assert (false, "VSTGUI_OVERRIDE_VMETHOD is deprecated, just use override!");
	#define VSTGUI_FINAL_VMETHOD static_assert (false, "VSTGUI_FINAL_VMETHOD is deprecated, just use final!");
#endif

//----------------------------------------------------
// Helper makros
//----------------------------------------------------
#define	VSTGUI_MAKE_STRING_PRIVATE_DONT_USE(x)	# x
#define	VSTGUI_MAKE_STRING(x)					VSTGUI_MAKE_STRING_PRIVATE_DONT_USE(x)

//----------------------------------------------------
#if DEVELOPMENT
	#ifndef DEBUG
		#define DEBUG	1
	#endif
#else
	#if !defined(NDEBUG) && !defined(DEBUG)
		#define NDEBUG	1
	#endif
#endif

//----------------------------------------------------
#define CLASS_METHODS(name, parent) CBaseObject* newCopy () const override { return new name (*this); }
#define CLASS_METHODS_NOCOPY(name, parent) CBaseObject* newCopy () const override { return nullptr; }
#define CLASS_METHODS_VIRTUAL(name, parent) CBaseObject* newCopy () const override = 0;

//----------------------------------------------------
namespace VSTGUI {

/** coordinate type */
using CCoord = double;
/** ID String pointer */
using IdStringPtr = const char*;
/** UTF8 String pointer */
using UTF8StringPtr = const char*;
/** UTF8 String buffer pointer */
using UTF8StringBuffer = char*;

//-----------------------------------------------------------------------------
// @brief Byte Order
//-----------------------------------------------------------------------------
enum ByteOrder
{
	kBigEndianByteOrder = 0,
	kLittleEndianByteOrder,
#if WINDOWS || defined(__LITTLE_ENDIAN__) || defined(__LITTLE_ENDIAN)
	kNativeByteOrder = kLittleEndianByteOrder
#else
	kNativeByteOrder = kBigEndianByteOrder
#endif
};

//-----------------------------------------------------------------------------
// @brief Message Results
//-----------------------------------------------------------------------------
enum CMessageResult
{
	kMessageUnknown = 0,
	kMessageNotified = 1
};

//-----------------------------------------------------------------------------
class IReference
{
public:
	/** decrease refcount and delete object if refcount == 0 */
	virtual void forget () = 0;
	/** increase refcount */
	virtual void remember () = 0;
};

//-----------------------------------------------------------------------------
template <typename T>
class ReferenceCounted : virtual public IReference
{
public:
	ReferenceCounted () = default;
	virtual ~ReferenceCounted () noexcept = default;

	ReferenceCounted (const ReferenceCounted&) {}
	ReferenceCounted& operator= (const ReferenceCounted&) { return *this; }

	//-----------------------------------------------------------------------------
	/// @name Reference Counting Methods
	//-----------------------------------------------------------------------------
	//@{
	void forget () override { if (--nbReference == 0) { beforeDelete (); delete this; } }
	void remember () override { nbReference++; }
	/** get refcount */
	virtual int32_t getNbReference () const { return nbReference; }
	//@}
private:
	virtual void beforeDelete () {}

	T nbReference {1};
};

using AtomicReferenceCounted = ReferenceCounted<std::atomic<int32_t>>;
using NonAtomicReferenceCounted = ReferenceCounted<int32_t>;

//-----------------------------------------------------------------------------
class IPlatformString : public AtomicReferenceCounted
{
public:
    virtual void setUTF8String (UTF8StringPtr utf8String) = 0;
};

//-----------------------------------------------------------------------------
// CBaseObject Declaration
//! @brief Base Object with reference counter
//-----------------------------------------------------------------------------
class CBaseObject : public NonAtomicReferenceCounted
{
public:
	CBaseObject () = default;
	~CBaseObject () noexcept override = default;

	CBaseObject (const CBaseObject&) {}
	CBaseObject& operator= (const CBaseObject&) { return *this; }

	//-----------------------------------------------------------------------------
	/// @name Message Methods
	//-----------------------------------------------------------------------------
	//@{
	virtual CMessageResult notify ([[maybe_unused]] CBaseObject* sender,
								   [[maybe_unused]] IdStringPtr message)
	{
		return kMessageUnknown;
	}
	//@}

	/// @cond ignore
	virtual CBaseObject* newCopy () const { return nullptr; }
	/// @endcond
};

//------------------------------------------------------------------------
template <class I>
class SharedPointer
{
public:
//------------------------------------------------------------------------
	inline SharedPointer (I* ptr, bool remember = true) noexcept;
	inline SharedPointer (const SharedPointer&) noexcept;
	inline SharedPointer () noexcept;
	inline ~SharedPointer () noexcept;

	inline I* operator=(I* ptr) noexcept;
	inline SharedPointer<I>& operator=(const SharedPointer<I>& ) noexcept;

	inline operator I* ()  const noexcept { return ptr; }      // act as I*
	inline I* operator->() const noexcept { return ptr; }      // act as I*

	inline I* get () const noexcept { return ptr; }

	template<class T> T* cast () const { return dynamic_cast<T*> (ptr); }

	inline SharedPointer (SharedPointer<I>&& mp) noexcept;
	inline SharedPointer<I>& operator=(SharedPointer<I>&& mp) noexcept;

	template<typename T>
	inline SharedPointer (const SharedPointer<T>& op) noexcept
	{
		*this = static_cast<I*> (op.get ());
	}

	template<typename T>
	inline SharedPointer& operator= (const SharedPointer<T>& op) noexcept
	{
		*this = static_cast<I*> (op.get ());
		return *this;
	}

	template<typename T>
	inline SharedPointer (SharedPointer<T>&& op) noexcept
	{
		*this = std::move (op);
	}

	template<typename T>
	inline SharedPointer& operator= (SharedPointer<T>&& op) noexcept
	{
		if (ptr)
			ptr->forget ();
		ptr = op.ptr;
		op.ptr = nullptr;
		return *this;
	}

//------------------------------------------------------------------------
protected:
	template<typename T>
	friend class SharedPointer;

	I* ptr {nullptr};
};

//-----------------------------------------------------------------------------
class CBaseObjectGuard
{
public:
	explicit CBaseObjectGuard (CBaseObject* _obj) : obj (_obj) {}
protected:
	SharedPointer<CBaseObject> obj;
};


//------------------------------------------------------------------------
template <class I>
inline SharedPointer<I>::SharedPointer (I* _ptr, bool remember) noexcept
: ptr (_ptr)
{
	if (ptr && remember)
		ptr->remember ();
}

//------------------------------------------------------------------------
template <class I>
inline SharedPointer<I>::SharedPointer (const SharedPointer<I>& other) noexcept
: ptr (other.ptr)
{
	if (ptr)
		ptr->remember ();
}

//------------------------------------------------------------------------
template <class I>
inline SharedPointer<I>::SharedPointer () noexcept
: ptr (nullptr)
{}

//------------------------------------------------------------------------
template <class I>
inline SharedPointer<I>::~SharedPointer () noexcept
{
	if (ptr)
		ptr->forget ();
}

//------------------------------------------------------------------------
template <class I>
inline SharedPointer<I>::SharedPointer (SharedPointer<I>&& mp) noexcept
: ptr (nullptr)
{
	*this = std::move (mp);
}

//------------------------------------------------------------------------
template <class I>
inline SharedPointer<I>& SharedPointer<I>::operator=(SharedPointer<I>&& mp) noexcept
{
	if (ptr)
		ptr->forget ();
	ptr = mp.ptr;
	mp.ptr = nullptr;
	return *this;
}

//------------------------------------------------------------------------
template <class I>
inline I* SharedPointer<I>::operator=(I* _ptr) noexcept
{
	if (_ptr != ptr)
	{
		if (ptr)
			ptr->forget ();
		ptr = _ptr;
		if (ptr)
			ptr->remember ();
	}
	return ptr;
}

//------------------------------------------------------------------------
template <class I>
inline SharedPointer<I>& SharedPointer<I>::operator=(const SharedPointer<I>& _ptr) noexcept
{
	operator= (_ptr.ptr);
	return *this;
}

//------------------------------------------------------------------------
template <class I>
inline SharedPointer<I> owned (I* p) noexcept { return SharedPointer<I> (p, false); }

//------------------------------------------------------------------------
template <class I>
inline SharedPointer<I> shared (I* p) noexcept { return SharedPointer<I> (p, true); }

//------------------------------------------------------------------------
template <class I, typename ...Args>
inline SharedPointer<I> makeOwned (Args&& ...args)
{
	return SharedPointer<I> (new I (std::forward<Args>(args)...), false);
}

#if VSTGUI_ENABLE_DEPRECATED_METHODS
//------------------------------------------------------------------------
/** An owning pointer. \deprecated
 *
 *	This class is now deprecated. Please change your code from
 *	@code{.cpp} OwningPointer<MyClass> obj = new MyClass () @endcode
 *	to
 *	@code{.cpp} SharedPointer<MyClass> obj = makeOwned<MyClass> (); @endcode
 */
template <class I>
class OwningPointer : public SharedPointer<I>
{
public:
//------------------------------------------------------------------------
	inline OwningPointer (I* p) : SharedPointer<I> (p, false) {}
	inline OwningPointer (const SharedPointer<I>& p) : SharedPointer<I> (p) {}
	inline OwningPointer (const OwningPointer<I>& p) : SharedPointer<I> (p) {}
	inline OwningPointer () : SharedPointer<I> () {}
	inline I* operator=(I* _ptr)
	{
		if (_ptr != this->ptr)
		{
			if (this->ptr)
				this->ptr->forget ();
			this->ptr = _ptr;
		}
		return this->ptr;
	}
};
#endif

//------------------------------------------------------------------------
template <typename T, typename B>
inline void setBit (T& storage, B bit, bool state)
{
	static_assert (std::is_integral<T>::value, "only works for integral types");
	static_assert (sizeof (T) >= sizeof (B), "bit type is too big");
	if (state)
		storage |= static_cast<T> (bit);
	else
		storage &= ~(static_cast<T> (bit));
}

//------------------------------------------------------------------------
template <typename T, typename B>
inline constexpr bool hasBit (T storage, B bit)
{
	static_assert (std::is_integral<T>::value, "only works for integral types");
	static_assert (sizeof (T) >= sizeof (B), "bit type is too big");
	return (storage & static_cast<T> (bit)) ? true : false;
}

//-----------------------------------------------------------------------------
template <typename T, typename B>
struct BitScopeToggleT
{
	BitScopeToggleT (T& storage, B bit) : storage (storage), bit (bit) { toggle (); }
	~BitScopeToggleT () noexcept { toggle (); }

private:
	void toggle ()
	{
		bool state = hasBit (storage, bit);
		setBit (storage, bit, !state);
	}
	T& storage;
	B bit;
};

//-----------------------------------------------------------------------------
#define VSTGUI_NEWER_THAN(major, minor) \
	(VSTGUI_VERSION > major || (VSTGUI_VERSION_MAJOR == major && VSTGUI_VERSION_MINOR > minor))

#define VSTGUI_NEWER_THAN_4_10 VSTGUI_NEWER_THAN (4, 10)
#define VSTGUI_NEWER_THAN_4_11 VSTGUI_NEWER_THAN (4, 11)
#define VSTGUI_NEWER_THAN_4_12 VSTGUI_NEWER_THAN (4, 12)
#define VSTGUI_NEWER_THAN_4_13 VSTGUI_NEWER_THAN (4, 13)




//-----------------------------------------------------------------------------
using CViewAttributeID = size_t;
using ModalViewSessionID = uint32_t;

//-----------------------------------------------------------------------------
static constexpr uint32_t kStreamIOError = (std::numeric_limits<uint32_t>::max) ();
static constexpr int64_t kStreamSeekError = -1;

// enums
//----------------------------
// @brief Mouse Wheel Axis
//----------------------------
enum CMouseWheelAxis
{
    kMouseWheelAxisX = 0,
    kMouseWheelAxisY
};

//----------------------------
// @brief Mouse Event Results
//----------------------------
enum CMouseEventResult
{
    kMouseEventNotImplemented = 0,
    kMouseEventHandled,
    kMouseEventNotHandled,
    kMouseDownEventHandledButDontNeedMovedOrUpEvents,
    kMouseMoveEventHandledButDontNeedMoreEvents
};

//----------------------------
// @brief Cursor Type
//----------------------------
enum CCursorType
{
    /** arrow cursor */
    kCursorDefault = 0,
    /** wait cursor */
    kCursorWait,
    /** horizontal size cursor */
    kCursorHSize,
    /** vertical size cursor */
    kCursorVSize,
    /** size all cursor */
    kCursorSizeAll,
    /** northeast and southwest size cursor */
    kCursorNESWSize,
    /** northwest and southeast size cursor */
    kCursorNWSESize,
    /** copy cursor (mainly for drag&drop operations) */
    kCursorCopy,
    /** not allowed cursor (mainly for drag&drop operations) */
    kCursorNotAllowed,
    /** hand cursor */
    kCursorHand,
    /** i beam cursor */
    kCursorIBeam,
    /** crosshair cursor */
    kCursorCrosshair,
};

//----------------------------
// @brief View Autosizing
//----------------------------
enum CViewAutosizing
{
    kAutosizeNone			= 0,
    kAutosizeLeft			= 1 << 0,
    kAutosizeTop			= 1 << 1,
    kAutosizeRight			= 1 << 2,
    kAutosizeBottom			= 1 << 3,
    kAutosizeColumn			= 1 << 4,
    kAutosizeRow			= 1 << 5,
    kAutosizeAll			= kAutosizeLeft | kAutosizeTop | kAutosizeRight | kAutosizeBottom
};

enum DragResult {
    kDragRefused = 0,
    kDragMoved,
    kDragCopied,
    kDragError = -1
};

//-----------
// @brief Text Face
//-----------
enum CTxtFace
{
    kNormalFace = 0,
    kBoldFace = 1 << 1,
    kItalicFace = 1 << 2,
    kUnderlineFace = 1 << 3,
    kStrikethroughFace = 1 << 4
};

//----------------------------
// @brief Bitmap Interpolation
//----------------------------
enum class BitmapInterpolationQuality
{
    kDefault = 0,	///< Let system decide
    kLow,			///< Nearest neighbour
    kMedium,		///< Bilinear interpolation
    kHigh			///< Bicubic interpolation (Bilinear on Windows)
};

//-----------------------------------------------------------------------------
enum class CSliderMode
{
    Touch,
    RelativeTouch,
    FreeClick,
    Ramp,
    UseGlobal
};

enum class DragOperation;

// @brief Stream seek modes
enum class SeekMode
{
    Set,
    Current,
    End
};

// simple structs
struct CColor;
struct CPoint;
struct CRect;
struct CButtonState;
struct CDrawMode;
struct CGraphicsTransform;
struct DragDescription;
struct DragEventData;
struct ModalViewSession;
struct CListControlRowDesc;
struct CNinePartTiledDescription;
struct CMultiFrameBitmapDescription;

using GradientColorStop = std::pair<double, CColor>;
using GradientColorStopMap = std::multimap<double, CColor>;

using LinePair = std::pair<CPoint, CPoint>;
using LineList = std::vector<LinePair>;
using PointList = std::vector<CPoint>;

// interfaces
class IViewListener;
class IViewEventListener;
class IViewContainerListener;
class IViewMouseListener;
class IDataPackage;
class IDependency;
class IFocusDrawing;
class IScaleFactorChangedListener;
class IDataBrowserDelegate;
class IMouseObserver;
class IKeyboardHook;
class IViewAddedRemovedObserver;
class IFocusViewObserver;
class ISplitViewController;
class ISplitViewSeparatorDrawer;
class IScrollbarDrawer;
class IControlListener;
class IDragCallback;
class IDraggingSession;
class IDropTarget;
class ICommandMenuItemTarget;
class IOptionMenuListener;
class ITextEditListener;
class ITextLabelListener;
class IListControlDrawer;
class IListControlConfigurator;

#if VSTGUI_TOUCH_EVENT_HANDLING
class ITouchEvent;
#endif

// classes
class CBitmap;
class CMultiFrameBitmap;
class CNinePartTiledBitmap;
class CResourceDescription;
class CLineStyle;
class CDrawContext;
class COffscreenContext;
class CDropSource;
class CFileExtension;
class CNewFileSelector;
class CFontDesc;
class VSTGUIEditorInterface;
class CTooltipSupport;
class CGraphicsPath;
class CGradient;
class UTF8String;
class UTF8StringView;
class CVSTGUITimer;
class CMenuItem;
class CCommandMenuItem;
class GenericStringListDataBrowserSource;
class StaticListControlConfigurator;
class StringListControlDrawer;

using CFontRef = CFontDesc*;

// views
class CFrame;
class CDataBrowser;
class CGradientView;
class CLayeredViewContainer;
class CAutoLayoutContainerView;
class CRowColumnView;
class CScrollView;
class CShadowViewContainer;
class CSplitView;
class CTabView;
class CView;
class CViewContainer;
#if VSTGUI_OPENGL_SUPPORT
class COpenGLView;
#endif

// controls
class CAutoAnimation;
class COnOffButton;
class CCheckBox;
class CKickButton;
class CTextButton;
class CColorChooser;
class CControl;
class CFontChooser;
class CKnob;
class CAnimKnob;
class CMovieBitmap;
class CMovieButton;
class COptionMenu;
class CParamDisplay;
class CScrollbar;
class CSegmentButton;
class CSlider;
class CVerticalSlider;
class CHorizontalSlider;
class CSpecialDigit;
class CSplashScreen;
class CAnimationSplashScreen;
class CVerticalSwitch;
class CHorizontalSwitch;
class CRockerSwitch;
class CTextEdit;
class CSearchTextEdit;
class CTextLabel;
class CMultiLineTextLabel;
class CVuMeter;
class CXYPad;
class CListControl;

// events
struct Event;
struct ModifierEvent;
struct MousePositionEvent;
struct MouseEvent;
struct MouseDownUpMoveEvent;
struct MouseDownEvent;
struct MouseMoveEvent;
struct MouseUpEvent;
struct MouseCancelEvent;
struct MouseEnterEvent;
struct MouseExitEvent;
struct GestureEvent;
struct MouseWheelEvent;
struct ZoomGestureEvent;
struct KeyboardEvent;
struct Modifiers;
enum class EventType : uint32_t;
enum class VirtualKey : uint32_t;
enum class ModifierKey : uint32_t;

const Event& noEvent ();

// animation
namespace Animation {
class IAnimationTarget;
class ITimingFunction;
class AlphaValueAnimation;
class ViewSizeAnimation;
class ExchangeViewAnimation;
class ControlValueAnimation;
class Animator;
class TimingFunctionBase;
class LinearTimingFunction;
class PowerTimingFunction;
class InterpolationTimingFunction;
class RepeatTimingFunction;
using DoneFunction = std::function<void (CView*, const IdStringPtr, IAnimationTarget*)>;
} // Animation

template <class I> class SharedPointer;

// platform
class IPlatformTextEdit;
class IPlatformTextEditCallback;
class IPlatformOptionMenu;
class IPlatformOpenGLView;
class IPlatformViewLayer;
class IPlatformViewLayerDelegate;
class IPlatformString;
class IPlatformBitmap;
class IPlatformBitmapPixelAccess;
class IPlatformFont;
class IPlatformFrame;
class IFontPainter;
class IPlatformResourceInputStream;

class IPlatformFactory;
class IPlatformFrame;
class IPlatformBitmap;
class IPlatformFont;
class IPlatformGradient;
class IPlatformGraphicsPath;
class IPlatformGraphicsPathFactory;
class IPlatformString;
class IPlatformTimer;
class IPlatformResourceInputStream;
class IPlatformFrameConfig;
class IPlatformFrameCallback;
class IPlatformTimerCallback;
class IPlatformFileSelector;
class IPlatformGraphicsDeviceFactory;
class IPlatformGraphicsDevice;
class IPlatformGraphicsDeviceContext;
class IPlatformGraphicsDeviceContextBitmapExt;

struct PlatformFileExtension;
struct PlatformFileSelectorConfig;

enum class PlatformType : int32_t;
enum class PlatformGraphicsPathFillMode : int32_t;
enum class PlatformFileSelectorStyle : uint32_t;
enum class PlatformFileSelectorFlags : uint32_t;

using PlatformFramePtr = SharedPointer<IPlatformFrame>;
using PlatformBitmapPtr = SharedPointer<IPlatformBitmap>;
using PlatformFontPtr = SharedPointer<IPlatformFont>;
using PlatformStringPtr = SharedPointer<IPlatformString>;
using PlatformTimerPtr = SharedPointer<IPlatformTimer>;
using PlatformResourceInputStreamPtr = std::unique_ptr<IPlatformResourceInputStream>;
using PlatformFactoryPtr = std::unique_ptr<IPlatformFactory>;
using PlatformGradientPtr = std::unique_ptr<IPlatformGradient>;
using PlatformGraphicsPathPtr = std::unique_ptr<IPlatformGraphicsPath>;
using PlatformGraphicsPathFactoryPtr = std::shared_ptr<IPlatformGraphicsPathFactory>;
using PlatformFileSelectorPtr = std::shared_ptr<IPlatformFileSelector>;
using PlatformGraphicsDevicePtr = std::shared_ptr<IPlatformGraphicsDevice>;
using PlatformGraphicsDeviceContextPtr = std::shared_ptr<IPlatformGraphicsDeviceContext>;




//-----------------------------------------------------------------------------
using AssertionHandler = void (*) (const char* filename, const char* line, const char* condition,
                                   const char* desc);
void setAssertionHandler (AssertionHandler handler);
bool hasAssertionHandler ();
void doAssert (const char* filename, const char* line, const char* condition,
               const char* desc = nullptr) noexcept (false);




//-----------------------------------------------------------------------------
template<typename BaseIterator>
class UTF8CodePointIterator
{
public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = char32_t;
    using difference_type = ptrdiff_t;
    using pointer = char32_t*;
    using reference = char32_t&;

    using CodePoint = value_type;

    UTF8CodePointIterator () = default;
    UTF8CodePointIterator (const UTF8CodePointIterator& o) noexcept : it (o.it) {}
    explicit UTF8CodePointIterator (const BaseIterator& iterator) noexcept : it (iterator) {}

    UTF8CodePointIterator& operator++ () noexcept;
    UTF8CodePointIterator& operator-- () noexcept;
    UTF8CodePointIterator operator++ (int) noexcept;
    UTF8CodePointIterator operator-- (int) noexcept;

    bool operator== (const UTF8CodePointIterator& other) const noexcept;
    bool operator!= (const UTF8CodePointIterator& other) const noexcept;

    CodePoint operator* () const noexcept;

    BaseIterator base () const noexcept { return it; }
private:
    BaseIterator it;

    static constexpr uint8_t kFirstBitMask = 128u; // 1000000
    static constexpr uint8_t kSecondBitMask = 64u; // 0100000
    static constexpr uint8_t kThirdBitMask = 32u; // 0010000
    static constexpr uint8_t kFourthBitMask = 16u; // 0001000
    static constexpr uint8_t kFifthBitMask = 8u; // 0000100
};

/**
 *  @brief holds an UTF8 encoded string and a platform representation of it
 *
 */
//-----------------------------------------------------------------------------
class UTF8String
{
public:
    using StringType = std::string;
    using SizeType = std::string::size_type;
    using CodePointIterator = UTF8CodePointIterator<std::string::const_iterator>;

    UTF8String (UTF8StringPtr str = nullptr);
    UTF8String (const UTF8String& other);
    explicit UTF8String (const StringType& str);
    UTF8String (UTF8String&& other) noexcept;
    UTF8String (StringType&& str) noexcept;

    UTF8String& operator= (const UTF8String& other);
    UTF8String& operator= (const StringType& other);
    UTF8String& operator= (UTF8String&& other) noexcept;
    UTF8String& operator= (StringType&& str) noexcept;
    UTF8String& operator= (UTF8StringPtr str) { assign (str); return *this; }

    SizeType length () const noexcept { return string.length (); }
    bool empty () const noexcept { return string.empty (); }

    void copy (UTF8StringBuffer dst, SizeType dstSize) const noexcept;

    CodePointIterator begin () const noexcept;
    CodePointIterator end () const noexcept;

    bool operator== (UTF8StringPtr str) const noexcept;
    bool operator!= (UTF8StringPtr str) const noexcept;
    bool operator== (const UTF8String& str) const noexcept;
    bool operator!= (const UTF8String& str) const noexcept;
    bool operator== (const StringType& str) const noexcept;
    bool operator!= (const StringType& str) const noexcept;

    UTF8String& operator+= (const UTF8String& other);
    UTF8String& operator+= (StringType::value_type ch);
    UTF8String& operator+= (const StringType::value_type* other);
    UTF8String operator+ (const UTF8String& other);
    UTF8String operator+ (StringType::value_type ch);
    UTF8String operator+ (const StringType::value_type* other);

    void assign (UTF8StringPtr str);
    void clear () noexcept;

    UTF8StringPtr data () const noexcept { return string.data (); }
    operator const UTF8StringPtr () const noexcept { return data (); }
    const StringType& getString () const noexcept { return string; }
    IPlatformString* getPlatformString () const noexcept;

    explicit operator bool () const = delete;
//-----------------------------------------------------------------------------
private:
    StringType string;
    mutable PlatformStringPtr platformString;
};

inline bool operator== (const UTF8String::StringType& lhs, const UTF8String& rhs) noexcept { return lhs == rhs.getString (); }
inline bool operator!= (const UTF8String::StringType& lhs, const UTF8String& rhs) noexcept { return lhs != rhs.getString (); }

inline UTF8String operator+ (UTF8StringPtr lhs, const UTF8String& rhs) { return UTF8String (lhs) += rhs; }

//-----------------------------------------------------------------------------
template<typename T>
inline UTF8String toString (const T& value)
{
    return UTF8String (std::to_string (value));
}

//-----------------------------------------------------------------------------
/** white-character test
 *	@param character UTF-32 character
 *	@return true if character is a white-character
 */
bool isSpace (char32_t character) noexcept;

//-----------------------------------------------------------------------------
struct TrimOptions
{
    using CharTestFunc = std::function<bool (char32_t)>;
    TrimOptions (CharTestFunc&& func = [] (char32_t c) { return isSpace (c); }) : test (std::move (func)) {}

    TrimOptions& left () { setBit (flags, Flags::kLeft, true); return *this; }
    TrimOptions& right () { setBit (flags, Flags::kRight, true); return *this; }

    bool trimLeft () const { return hasBit (flags, Flags::kLeft); }
    bool trimRight () const { return hasBit (flags, Flags::kRight); }

    bool operator() (char32_t c) const { return !test (c); }

private:
    enum Flags : uint8_t {
        kLeft = 1 << 0,
        kRight = 1 << 1
    };
    uint8_t flags {0};
    CharTestFunc test;
};

//-----------------------------------------------------------------------------
UTF8String trim (const UTF8String& str, TrimOptions options = TrimOptions ().left ().right ());

#if VSTGUI_ENABLE_DEPRECATED_METHODS
//-----------------------------------------------------------------------------
namespace String {
    VSTGUI_DEPRECATED(/** @deprecated Allocates a new UTF8StringBuffer with enough size for string and copy the string into it. Returns nullptr if string is a nullptr. */
    UTF8StringBuffer newWithString (UTF8StringPtr string);)
    VSTGUI_DEPRECATED(/** @deprecated Frees an UTF8StringBuffer. If buffer is a nullptr it does nothing. */
    void free (UTF8StringBuffer buffer);)
}
#endif

//-----------------------------------------------------------------------------
/** @brief a view on a null terminated UTF-8 String

    It does not copy the string.
    It's allowed to put null pointers into it.
    A null pointer is treaded different than an empty string as they are not equal and the byte
    count of a null pointer is zero while the empty string has a byte count of one.
*/
//-----------------------------------------------------------------------------
class UTF8StringView
{
public:
    UTF8StringView () : str (nullptr), byteCount (0) {}
    UTF8StringView (const UTF8StringPtr string) : str (string) {}
    UTF8StringView (const UTF8String& string) : str (string.data ()), byteCount (string.length () + 1) {}
    UTF8StringView (const std::string& string) : str (string.data ()), byteCount (string.size () + 1) {}

    UTF8StringView (const UTF8StringView& other) noexcept;
    UTF8StringView& operator= (const UTF8StringView& other) noexcept;
    UTF8StringView (UTF8StringView&& other) noexcept = default;
    UTF8StringView& operator= (UTF8StringView&& other) = default;

    /** calculates the bytes used by this string, including null-character */
    size_t calculateByteCount () const;

    /** calculates the number of UTF-8 characters in the string */
    size_t calculateCharacterCount () const;

    /** checks this string if it contains a subString */
    bool contains (const UTF8StringPtr subString, bool ignoreCase = false) const;

    /** checks this string if it starts with startString */
    bool startsWith (const UTF8StringView& startString) const;

    /** checks this string if it ends with endString */
    bool endsWith (const UTF8StringView& endString) const;

    /** converts the string to a double */
    double toDouble (uint32_t precision = 8) const;

    /** converts the string to a float */
    float toFloat (uint32_t precision = 8) const;

    /** converts the string to an integer */
    int64_t toInteger () const;

    template<typename T>
    Optional<T> toNumber () const;

    bool operator== (const UTF8StringPtr otherString) const;
    bool operator!= (const UTF8StringPtr otherString) const;
    bool operator== (UTF8StringView otherString) const;
    bool operator!= (UTF8StringView otherString) const;
    bool operator== (const UTF8String& otherString) const;
    bool operator!= (const UTF8String& otherString) const;
    operator const UTF8StringPtr () const;
//-----------------------------------------------------------------------------
private:
    UTF8StringPtr str;
    mutable Optional<size_t> byteCount;
};

//-----------------------------------------------------------------------------
class UTF8CharacterIterator
{
public:
    UTF8CharacterIterator (const UTF8StringPtr utf8Str)
    : startPos ((const uint8_t*)utf8Str)
    , currentPos (nullptr)
    , strLen (std::strlen (utf8Str))
    {
        begin ();
    }

    UTF8CharacterIterator (const UTF8StringPtr utf8Str, size_t strLen)
    : startPos ((const uint8_t*)utf8Str)
    , currentPos (nullptr)
    , strLen (strLen)
    {
        begin ();
    }

    UTF8CharacterIterator (const std::string& stdStr)
    : startPos ((const uint8_t*)stdStr.data ())
    , currentPos (nullptr)
    , strLen (stdStr.size ())
    {
        begin ();
    }

    const uint8_t* next ()
    {
        if (currentPos)
        {
            if (currentPos == back ())
            {}
            else if (*currentPos <= 0x7F) // simple ASCII character
                currentPos++;
            else
            {
                uint8_t characterLength = getByteLength ();
                if (characterLength)
                    currentPos += characterLength;
                else
                    currentPos = end (); // error, not an allowed UTF-8 character at this position
            }
        }
        return currentPos;
    }

    const uint8_t* previous ()
    {
        while (currentPos)
        {
            --currentPos;
            if (currentPos < front ())
            {
                currentPos = begin ();
                break;
            }
            else
            {
                if (*currentPos <= 0x7f || (*currentPos >= 0xC0 && *currentPos <= 0xFD))
                    break;
            }
        }
        return currentPos;
    }

    uint8_t getByteLength () const
    {
        if (currentPos && currentPos != back ())
        {
            if (*currentPos <= 0x7F)
                return 1;
            else
            {
                if (*currentPos >= 0xC0 && *currentPos <= 0xFD)
                {
                    if ((*currentPos & 0xF8) == 0xF8)
                        return 5;
                    else if ((*currentPos & 0xF0) == 0xF0)
                        return 4;
                    else if ((*currentPos & 0xE0) == 0xE0)
                        return 3;
                    else if ((*currentPos & 0xC0) == 0xC0)
                        return 2;
                }
            }
        }
        return 0;
    }

    const uint8_t* begin () { currentPos = startPos; return currentPos;}
    const uint8_t* end () { currentPos = startPos + strLen; return currentPos; }

    const uint8_t* front () const { return startPos; }
    const uint8_t* back () const { return startPos + strLen; }

    const uint8_t* operator++() { return next (); }
    const uint8_t* operator--() { return previous (); }
    bool operator==(uint8_t i) { if (currentPos) return *currentPos == i; return false; }
    operator const uint8_t* () const { return currentPos; }

protected:
    const uint8_t* startPos;
    const uint8_t* currentPos;
    size_t strLen;
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline UTF8StringView::UTF8StringView (const UTF8StringView& other) noexcept
{
    *this = other;
}

//------------------------------------------------------------------------
inline UTF8StringView& UTF8StringView::operator= (const UTF8StringView& other) noexcept
{
    str = other.str;
    if (other.byteCount)
        byteCount = makeOptional (*other.byteCount);
    return *this;
}

//------------------------------------------------------------------------
inline size_t UTF8StringView::calculateCharacterCount () const
{
    size_t count = 0;
    if (str == nullptr)
        return count;

    UTF8CharacterIterator it (str);
    while (it != it.back ())
    {
        count++;
        ++it;
    }
    return count;
}

//-----------------------------------------------------------------------------
inline size_t UTF8StringView::calculateByteCount () const
{
    if (!byteCount)
        byteCount = makeOptional<size_t> (str ? std::strlen (str) + 1 : 0);
    return *byteCount;
}

//-----------------------------------------------------------------------------
inline bool UTF8StringView::contains (const UTF8StringPtr subString, bool ignoreCase) const
{
    if (ignoreCase)
    {
        if (!str || !subString)
            return false;
        UTF8CharacterIterator subIt (subString);
        UTF8CharacterIterator it (str, calculateByteCount ());
        auto foundIt = std::search (
            it.begin (), it.end (), subIt.begin (), subIt.end (),
            [] (uint8_t c1, uint8_t c2) { return std::toupper (c1) == std::toupper (c2); });
        return foundIt != it.end ();
    }
    return (!str || !subString || std::strstr (str, subString) == nullptr) ? false : true;
}

//-----------------------------------------------------------------------------
inline bool UTF8StringView::startsWith (const UTF8StringView& startString) const
{
    if (!str || !startString.str)
        return false;
    size_t startStringLen = startString.calculateByteCount ();
    size_t thisLen = calculateByteCount ();
    if (startStringLen > thisLen)
        return false;
    return std::strncmp (str, startString.str, startStringLen - 1) == 0;
}

//-----------------------------------------------------------------------------
inline bool UTF8StringView::endsWith (const UTF8StringView& endString) const
{
    size_t endStringLen = endString.calculateByteCount ();
    size_t thisLen = calculateByteCount ();
    if (endStringLen > thisLen)
        return false;
    return endString == UTF8StringView (str + (thisLen - endStringLen));
}

//-----------------------------------------------------------------------------
inline float UTF8StringView::toFloat (uint32_t precision) const
{
    return static_cast<float>(toDouble (precision));
}

//------------------------------------------------------------------------
template<typename T>
inline Optional<T> UTF8StringView::toNumber () const
{
    auto number = toInteger ();
    if (number > std::numeric_limits<T>::max () || number < std::numeric_limits<T>::min ())
        return {};
    return makeOptional (static_cast<T> (number));
}

//------------------------------------------------------------------------
template<>
inline Optional<bool> UTF8StringView::toNumber () const
{
    auto number = toInteger ();
    if (number > 1 || number < 0)
        return {};
    return makeOptional (static_cast<bool> (number));
}

//-----------------------------------------------------------------------------
inline bool UTF8StringView::operator== (const UTF8StringPtr otherString) const
{
    if (str == otherString) return true;
    return (str && otherString) ? (std::strcmp (str, otherString) == 0) : false;
}

//-----------------------------------------------------------------------------
inline bool UTF8StringView::operator!= (const UTF8StringPtr otherString) const
{
    return !(*this == otherString);
}

//-----------------------------------------------------------------------------
inline bool UTF8StringView::operator== (UTF8StringView otherString) const
{
    if (byteCount && otherString.byteCount && *byteCount != *otherString.byteCount)
        return false;
    return operator==(otherString.str);
}

//------------------------------------------------------------------------
inline bool UTF8StringView::operator!= (UTF8StringView otherString) const
{
    if (byteCount && otherString.byteCount && *byteCount != *otherString.byteCount)
        return true;
    return operator!= (otherString.str);
}

//------------------------------------------------------------------------
inline bool UTF8StringView::operator== (const UTF8String& otherString) const
{
    return otherString == str;
}

//------------------------------------------------------------------------
inline bool UTF8StringView::operator!= (const UTF8String& otherString) const
{
    return otherString != str;
}

//-----------------------------------------------------------------------------
inline UTF8StringView::operator const UTF8StringPtr () const
{
    return str;
}

//-----------------------------------------------------------------------------
template<typename BaseIterator>
inline UTF8CodePointIterator<BaseIterator>& UTF8CodePointIterator<BaseIterator>::operator++ () noexcept
{
    auto firstByte = *it;

    difference_type offset = 1;

    if (firstByte & kFirstBitMask)
    {
        if (firstByte & kThirdBitMask)
        {
            if (firstByte & kFourthBitMask)
                offset = 4;
            else
                offset = 3;
        }
        else
        {
            offset = 2;
        }
    }
    it += offset;
    return *this;
}

//-----------------------------------------------------------------------------
template<typename BaseIterator>
inline UTF8CodePointIterator<BaseIterator>& UTF8CodePointIterator<BaseIterator>::operator-- () noexcept
{
    --it;
    if (*it & kFirstBitMask)
    {
        --it;
        if ((*it & kSecondBitMask) == 0)
        {
            --it;
            if ((*it & kSecondBitMask) == 0)
            {
                --it;
            }
        }
    }
    return *this;
}

//-----------------------------------------------------------------------------
template<typename BaseIterator>
inline UTF8CodePointIterator<BaseIterator> UTF8CodePointIterator<BaseIterator>::operator++ (int) noexcept
{
    auto result = *this;
    ++(*this);
    return result;
}

//-----------------------------------------------------------------------------
template<typename BaseIterator>
inline UTF8CodePointIterator<BaseIterator> UTF8CodePointIterator<BaseIterator>::operator-- (int) noexcept
{
    auto result = *this;
    --(*this);
    return result;
}

//-----------------------------------------------------------------------------
template<typename BaseIterator>
inline bool UTF8CodePointIterator<BaseIterator>::operator== (const UTF8CodePointIterator<BaseIterator>& other) const noexcept
{
    return it == other.it;
}

//-----------------------------------------------------------------------------
template<typename BaseIterator>
inline bool UTF8CodePointIterator<BaseIterator>::operator!= (const UTF8CodePointIterator<BaseIterator>& other) const noexcept
{
    return it != other.it;
}

//-----------------------------------------------------------------------------
template<typename BaseIterator>
inline typename UTF8CodePointIterator<BaseIterator>::CodePoint UTF8CodePointIterator<BaseIterator>::operator* () const noexcept
{
    CodePoint codePoint = 0;

    auto firstByte = *it;

    if (firstByte & kFirstBitMask)
    {
        if (firstByte & kThirdBitMask)
        {
            if (firstByte & kFourthBitMask)
            {
                codePoint = static_cast<CodePoint> ((firstByte & 0x07) << 18);
                auto secondByte = *(it + 1);
                codePoint +=  static_cast<CodePoint> ((secondByte & 0x3f) << 12);
                auto thirdByte = *(it + 2);
                codePoint +=  static_cast<CodePoint> ((thirdByte & 0x3f) << 6);
                auto fourthByte = *(it + 3);
                codePoint += (fourthByte & 0x3f);
            }
            else
            {
                codePoint = static_cast<CodePoint> ((firstByte & 0x0f) << 12);
                auto secondByte = *(it + 1);
                codePoint += static_cast<CodePoint> ((secondByte & 0x3f) << 6);
                auto thirdByte = *(it + 2);
                codePoint +=  static_cast<CodePoint> ((thirdByte & 0x3f));
            }
        }
        else
        {
            codePoint = static_cast<CodePoint> ((firstByte & 0x1f) << 6);
            auto secondByte = *(it + 1);
            codePoint +=  (secondByte & 0x3f);
        }
    }
    else
    {
        codePoint = static_cast<CodePoint> (firstByte);
    }
    return codePoint;
}

namespace Detail {

//-----------------------------------------------------------------------------
struct Locale
{
    Locale ()
    {
        origLocal = ::std::locale ();
        ::std::locale::global (::std::locale::classic ());
    }

    ~Locale () noexcept
    {
        ::std::locale::global (origLocal);
    }

    ::std::locale origLocal;
};

//------------------------------------------------------------------------
} // Detail

} // VSTGUI

// inline void vstgui_assert_impl(bool x, char* file, int line, char* txt)
// {
//     if (!(x))
//     {
//         VSTGUI::doAssert (file, VSTGUI_MAKE_STRING (line), txt, "...");
//     }
// }

// #ifndef vstgui_assert
// #define vstgui_assert(x) vstgui_assert_impl((x),__FILE__,__LINE__,VSTGUI_MAKE_STRING (x));
// #endif

// #ifndef vstgui_assert
// #define vstgui_assert(x)
// #endif

inline void vstgui_assert(bool x)
{
}
