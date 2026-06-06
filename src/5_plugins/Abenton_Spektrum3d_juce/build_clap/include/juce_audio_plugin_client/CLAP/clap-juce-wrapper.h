/*
 * clap-juce-wrapper.cpp
 *
 * Released under the MIT License, as described in LICENSE.md in this repository
 */
#pragma once

#include <juce_core/juce_core.h>
#include <juce_core/juce_core_common.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_processors/format_types/juce_LegacyAudioParameter.h>
#include <clap/helpers/checking-level.hh>
#include <clap/helpers/context-menu-builder.hh>
#include <clap/helpers/host-proxy.hh>
#include <clap/helpers/host-proxy.hxx>
#include <clap/helpers/plugin.hh>
#include <clap/helpers/plugin.hxx>

#include <juce_audio_plugin_client/CLAP/clap-juce-extensions.h>
#include <juce_audio_plugin_client/CLAP/clap-juce-entry.h>
#include <JucePluginConfig.h>

#if 0

#include "clap-juce-entry.h"

#if _WIN32
#define _CRT_SECURE_NO_WARNINGS 1
#endif

#include <memory>
#include <unordered_map>
#include <unordered_set>
// #include <algorithm>
#include <new>


#define JUCE_GUI_BASICS_INCLUDE_XHEADERS 1
#include <juce_core/system/juce_CompilerWarnings.h>
#include <juce_core/system/juce_TargetPlatform.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

#if JUCE_VERSION >= 0x08000B
#include <juce_audio_processors_headless/juce_audio_processors_headless.h>
#include <juce_audio_processors_headless/format_types/juce_LegacyAudioParameter.h>
#else
#include <juce_audio_processors/format_types/juce_LegacyAudioParameter.h>
#endif

#if JUCE_VERSION >= 0x070006
#include <juce_audio_plugin_client/detail/juce_IncludeSystemHeaders.h>
#include <juce_audio_plugin_client/detail/juce_PluginUtilities.h>
#include <juce_audio_plugin_client/detail/juce_VSTWindowUtilities.h>
#endif

JUCE_BEGIN_IGNORE_WARNINGS_GCC_LIKE("-Wunused-parameter", "-Wsign-conversion", "-Wfloat-conversion",
                                    "-Wfloat-equal")
JUCE_BEGIN_IGNORE_WARNINGS_MSVC(4100 4127 4244)
// Sigh - X11.h eventually does a #define None 0L which doesn't work
// with an enum in clap land being called None, so just undef it
// post the JUCE installs
#ifdef None
#undef None
#endif
#include <clap/helpers/checking-level.hh>
#include <clap/helpers/context-menu-builder.hh>
#include <clap/helpers/host-proxy.hh>
#include <clap/helpers/host-proxy.hxx>
#include <clap/helpers/plugin.hh>
#include <clap/helpers/plugin.hxx>

#if CLAP_VERSION_LT(1, 2, 0)
static_assert(false, "CLAP juce wrapper requires at least clap 1.2.0");
#endif

JUCE_END_IGNORE_WARNINGS_MSVC
JUCE_END_IGNORE_WARNINGS_GCC_LIKE

#include "clap-juce-extensions.h"

#if JUCE_LINUX
#if JUCE_VERSION >= 0x070006
#include <vector>
#include <juce_events/native/juce_EventLoopInternal_linux.h>
#include <juce_audio_plugin_client/detail/juce_LinuxMessageThread.h>
#define HAS_LINUX_FD 1
#elif JUCE_VERSION > 0x060008
#include <juce_audio_plugin_client/utility/juce_LinuxMessageThread.h>
#endif
#endif

#define FIXME(x)                                                                                   \
    {                                                                                              \
        static bool onetime_ = false;                                                              \
        if (!onetime_)                                                                             \
        {                                                                                          \
            std::ostringstream oss;                                                                \
            oss << "FIXME: " << x << " @" << __LINE__;                                             \
            DBG(oss.str());                                                                        \
        }                                                                                          \
        jassert(onetime_);                                                                         \
        onetime_ = true;                                                                           \
    }

#if CLAP_SUPPORTS_CUSTOM_FACTORY
extern const void *JUCE_CALLTYPE clapJuceExtensionCustomFactory(const char *);
#endif

#if !JUCE_MAC
template <typename T> using Point = juce::Point<T>;
#if JUCE_VERSION < 0x070006
using Component = juce::Component;
#endif
#endif






#endif

/*
 * This is a utility lock free queue based on the JUCE abstract fifo
 */
template <typename T, int qSize = 4096> class PushPopQ
{
public:
    PushPopQ() : af(qSize) {}
    bool push(const T &ad)
    {
        auto ret = false;
        int start1, size1, start2, size2;
        af.prepareToWrite(1, start1, size1, start2, size2);
        if (size1 > 0)
        {
            dq[start1] = ad;
            ret = true;
        }
        af.finishedWrite(size1 + size2);
        return ret;
    }
    bool pop(T &ad)
    {
        bool ret = false;
        int start1, size1, start2, size2;
        af.prepareToRead(1, start1, size1, start2, size2);
        if (size1 > 0)
        {
            ad = dq[start1];
            ret = true;
        }
        af.finishedRead(size1 + size2);
        return ret;
    }
    juce::AbstractFifo af;
    T dq[(size_t)qSize];
};

#if JUCE_VERSION < 0x070006
/*
 * These functions are the JUCE VST2/3 NSView attachment functions. We compile them into
 * our clap dll by, on macos, also linking clap_juce_mac.mm
 */
namespace juce
{
extern JUCE_API void initialiseMacVST();
extern JUCE_API void *attachComponentToWindowRefVST(Component *, void *parentWindowOrView,
                                                    bool isNSView);
} // namespace juce
#endif

// Some compilers generate warnings when we use `strncpy` instead
// of `strncpy_s`. However, other compilers don't support `strncpy_s`.
// So for now, we ignore those warnings, but once all the compilers
// that we care about support `strncpy_s`, we should remove these
// warnings guards and use `strncpy_s`.
JUCE_BEGIN_IGNORE_WARNINGS_GCC_LIKE("-Wdeprecated-declarations")
JUCE_BEGIN_IGNORE_WARNINGS_MSVC(4996)

#if !defined(CLAP_MISBEHAVIOUR_HANDLER_LEVEL)
#define CLAP_MISBEHAVIOUR_HANDLER_LEVEL Ignore
#endif

#if !defined(CLAP_CHECKING_LEVEL)
#define CLAP_CHECKING_LEVEL Minimal
#endif

#if !defined(CLAP_PROCESS_EVENTS_RESOLUTION_SAMPLES)
#define CLAP_PROCESS_EVENTS_RESOLUTION_SAMPLES 0 // sample-accurate events are off by default
#endif

#if !defined(CLAP_ALWAYS_SPLIT_BLOCK)
#define CLAP_ALWAYS_SPLIT_BLOCK 0
#endif

#define CLAP_USE_JUCE_PARAMETER_RANGES_OFF 0
#define CLAP_USE_JUCE_PARAMETER_RANGES_DISCRETE 1
#define CLAP_USE_JUCE_PARAMETER_RANGES_ALL 2

#if !defined(CLAP_USE_JUCE_PARAMETER_RANGES)
#define CLAP_USE_JUCE_PARAMETER_RANGES CLAP_USE_JUCE_PARAMETER_RANGES_OFF
#endif

// This is useful for debugging overrides
// #undef CLAP_MISBEHAVIOUR_HANDLER_LEVEL
// #define CLAP_MISBEHAVIOUR_HANDLER_LEVEL Terminate
// #undef CLAP_CHECKING_LEVEL
// #define CLAP_CHECKING_LEVEL Maximal

/* Host context menus are only availble in JUCE 6.0.8 and later */
#if JUCE_VERSION >= 0x060008
class EditorContextMenu : public juce::HostProvidedContextMenu
{
    using HostType = clap::helpers::HostProxy<
        clap::helpers::MisbehaviourHandler::CLAP_MISBEHAVIOUR_HANDLER_LEVEL,
        clap::helpers::CheckingLevel::CLAP_CHECKING_LEVEL>;

  public:
    explicit EditorContextMenu(HostType &hostIn);

    juce::PopupMenu getEquivalentPopupMenu() const override;

    void showNativeMenu(juce::Point<int> pos) const override;

    clap_context_menu_target menuTarget{};

  private:
    HostType &host;

    struct MenuBuilder : clap::helpers::ContextMenuBuilder
    {
        int menuIDCounter = 0;
        std::vector<juce::PopupMenu> menuStack;

        juce::String currentSubMenuLabel;
        bool currentSubMenuEnabled = false;

        HostType &host;
        const clap_context_menu_target *menuTarget;

        MenuBuilder(HostType &h, const clap_context_menu_target *target)
            : host(h), menuTarget(target)
        {
            reset();
        }

        void reset()
        {
            menuIDCounter = 0;
            menuStack.clear();
            menuStack.emplace_back();
        }

        bool addItem(clap_context_menu_item_kind_t item_kind, const void *item_data) override
        {
            auto &currentMenu = menuStack.back();

            if (item_kind == CLAP_CONTEXT_MENU_ITEM_ENTRY)
            {
                const auto entry = static_cast<const clap_context_menu_entry *>(item_data);

                juce::PopupMenu::Item item;
                item.itemID = ++menuIDCounter;
                item.text = juce::CharPointer_UTF8(entry->label);
                item.isEnabled = entry->is_enabled;
                item.action = [&hostRef = this->host, target = *this->menuTarget,
                               id = entry->action_id] { hostRef.contextMenuPerform(&target, id); };

                currentMenu.addItem(item);
            }
            else if (item_kind == CLAP_CONTEXT_MENU_ITEM_CHECK_ENTRY)
            {
                const auto entry = static_cast<const clap_context_menu_check_entry *>(item_data);

                juce::PopupMenu::Item item;
                item.itemID = ++menuIDCounter;
                item.text = juce::CharPointer_UTF8(entry->label);
                item.isEnabled = entry->is_enabled;
                item.isTicked = entry->is_checked;
                item.action = [&hostRef = this->host, target = *this->menuTarget,
                               id = entry->action_id] { hostRef.contextMenuPerform(&target, id); };

                currentMenu.addItem(item);
            }
            else if (item_kind == CLAP_CONTEXT_MENU_ITEM_SEPARATOR)
            {
                currentMenu.addSeparator();
            }
            else if (item_kind == CLAP_CONTEXT_MENU_ITEM_BEGIN_SUBMENU)
            {
                const auto entry = static_cast<const clap_context_menu_submenu *>(item_data);

                // add a new menu to the stack for this sub-menu
                menuStack.emplace_back();

                // copy the sub-menu info for when we add it to the parent menu later
                currentSubMenuLabel = juce::CharPointer_UTF8(entry->label);
                currentSubMenuEnabled = entry->is_enabled;
            }
            else if (item_kind == CLAP_CONTEXT_MENU_ITEM_END_SUBMENU)
            {
                // copy current menu (which is a sub-menu)
                const auto subMenu = currentMenu;

                // pop the current menu from the stack
                jassert(menuStack.size() > 1); // trying to end a sub-menu that we didn't start?
                menuStack.pop_back();

                // add the sub-menu to the menu one level up
                menuStack.back().addSubMenu(currentSubMenuLabel, subMenu, currentSubMenuEnabled);
            }
            else if (item_kind == CLAP_CONTEXT_MENU_ITEM_TITLE)
            {
                const auto entry = static_cast<const clap_context_menu_item_title *>(item_data);
                currentMenu.addSectionHeader(juce::CharPointer_UTF8(entry->title));
                // CLAP allows a title to be disabled, but JUCE doesn't,
                // so for now we'll just say that titles are always enabled.
            }

            return true;
        }

        // Currently, JUCE supports all the item kinds that CLAP supports!
        bool supports(clap_context_menu_item_kind_t /*item_kind*/) const noexcept override
        {
            return true;
        }
    };
    MenuBuilder builder{host, &menuTarget};
};

class EditorHostContext : public juce::AudioProcessorEditorHostContext
{
    using HostProxyType = clap::helpers::HostProxy<
        clap::helpers::MisbehaviourHandler::CLAP_MISBEHAVIOUR_HANDLER_LEVEL,
        clap::helpers::CheckingLevel::CLAP_CHECKING_LEVEL>;

  public:
    EditorHostContext(
        HostProxyType &hostProxyIn,
          const std::unordered_map<const juce::AudioProcessorParameter *, clap_id> &paramMapIn);

    std::unique_ptr<juce::HostProvidedContextMenu>
    getContextMenuForParameter(const juce::AudioProcessorParameter *parameter) const
#if JUCE_VERSION > 0x060105
        override
#endif
        ;


#if JUCE_VERSION <= 0x060105
    std::unique_ptr<juce::HostProvidedContextMenu>
    getContextMenuForParameterIndex(const juce::AudioProcessorParameter *parameter) const override;
#endif

  private:
    HostProxyType &hostProxy;
    const std::unordered_map<const juce::AudioProcessorParameter *, clap_id> &paramMap;
};
#endif // JUCE_VERSION >= 0x060008

/** Converts a clap_color to a juce::Colour */
// static juce::Colour clapColourToJUCEColour(const clap_color &clapColour)
// {
//     return {clapColour.red, clapColour.green, clapColour.blue, clapColour.alpha};
// }

/*
 * The ClapJuceWrapper is a class which immplements a collection
 * of CLAP and JUCE APIs
 */
class ClapJuceWrapper : public clap::helpers::Plugin<
                            clap::helpers::MisbehaviourHandler::CLAP_MISBEHAVIOUR_HANDLER_LEVEL,
                            clap::helpers::CheckingLevel::CLAP_CHECKING_LEVEL>,
                        public juce::AudioProcessorListener,
                        public juce::AudioPlayHead,
                        public juce::AudioProcessorParameter::Listener,
#if HAS_LINUX_FD
                        public juce::LinuxEventLoopInternal::Listener,
#endif
                        public juce::ComponentListener
{
public:
    // this needs to be the very last thing to get deleted!
    juce::ScopedJuceInitialiser_GUI libraryInitializer;

    // TODO: Benni
    //static clap_plugin_descriptor desc;
    std::unique_ptr<juce::AudioProcessor> processor;
    clap_juce_extensions::clap_properties *processorAsClapProperties{nullptr};
    clap_juce_extensions::clap_juce_audio_processor_capabilities *processorAsClapExtensions{
        nullptr};

    bool usingLegacyParameterAPI{false};
    std::atomic<bool> callLatencyChangeOnNextActivate{false};

    ClapJuceWrapper(const clap_host *host, juce::AudioProcessor *p);
    ~ClapJuceWrapper() override;
    bool init() noexcept override;
    void reset() noexcept override;
public:
    bool implementsTimerSupport() const noexcept override;
    void onTimer(clap_id timerId) noexcept override;
#if HAS_LINUX_FD
    std::vector<int> registeredFDs;
    void fdCallbacksChanged() override;
    void unregisterExtantFDs();
    bool implementsPosixFdSupport() const noexcept override;
    void onPosixFd(int fd, clap_posix_fd_flags_t /* flags */) noexcept override;
#endif

    clap_id idleTimer{0};

    static uint32_t generateClapIDForJuceParam(juce::AudioProcessorParameter *param);

#if JUCE_VERSION >= 0x060008
    void audioProcessorChanged(juce::AudioProcessor *proc, const ChangeDetails &details) override;
#else
    void audioProcessorChanged(juce::AudioProcessor *proc) override;
#endif

    clap_id clapIdFromParameterIndex(int index) const;

    static float getUnNormalisedParameterValue(const JUCEParameterVariant &parameter, float value);

    static float getNormalisedParameterValue(const JUCEParameterVariant &parameter, float value);

    bool supressParameterChangeMessages{false};
    void audioProcessorParameterChanged(juce::AudioProcessor *, int index, float newValue) override;

    void audioProcessorParameterChangeGestureBegin(juce::AudioProcessor *, int index) override;

    void audioProcessorParameterChangeGestureEnd(juce::AudioProcessor *, int index) override;

#if JUCE_VERSION < 0x070000
    /*
     * According to the JUCE docs this is *only* called on the processing thread
     */
    bool getCurrentPosition(juce::AudioPlayHead::CurrentPositionInfo &info) override;
#else
    juce::Optional<juce::AudioPlayHead::PositionInfo> getPosition() const override;
#endif

    void parameterValueChanged(int, float newValue) override;

    void parameterGestureChanged(int, bool) override;

    bool cacheHostCanUseThreadCheck{false};
    bool activate(double sampleRate, uint32_t minFrameCount, uint32_t maxFrameCount) noexcept override;
    void deactivate() noexcept override;

    /* CLAP API */

    void defineAudioPorts();

  protected:
    bool startProcessing() noexcept override;
    void stopProcessing() noexcept override;
  public:
    bool implementsAudioPorts() const noexcept override;
    uint32_t audioPortsCount(bool isInput) const noexcept override;
    bool audioPortsInfo(uint32_t index, bool isInput, clap_audio_port_info *info) const noexcept override;
    uint32_t audioPortsConfigCount() const noexcept override;
    bool audioPortsGetConfig(uint32_t /*index*/, clap_audio_ports_config * /*config*/) const noexcept override;
    bool audioPortsSetConfig(clap_id /*configId*/) noexcept override;
    bool implementsNotePorts() const noexcept override;
    uint32_t notePortsCount(bool is_input) const noexcept override;
    bool notePortsInfo(uint32_t index, bool is_input, clap_note_port_info *info) const noexcept override;
    bool implementsVoiceInfo() const noexcept override;
    bool voiceInfoGet(clap_voice_info *info) noexcept override;
    bool implementsNoteName() const noexcept override;
    uint32_t noteNameCount() noexcept override;
    bool noteNameGet(uint32_t index, clap_note_name *noteName) noexcept override;
    bool implementsTrackInfo() const noexcept override;
    void trackInfoChanged() noexcept override;
    bool implementsParamIndication() const noexcept override;
    void paramIndicationSetMapping(clap_id param_id, bool has_mapping, const clap_color_t *color,
                                   const char *label, const char *description) noexcept override;
    void paramIndicationSetAutomation(clap_id param_id, uint32_t automation_state,
                                      const clap_color_t *color) noexcept override;
    bool implementRemoteControls() const noexcept override;
    uint32_t remoteControlsPageCount() noexcept override;
    bool remoteControlsPageGet(uint32_t pageIndex, clap_remote_controls_page *page) noexcept override;
    bool implementsPresetLoad() const noexcept override;
    bool presetLoadFromLocation(uint32_t location_kind, const char *location, const char *load_key) noexcept override;

  public:
    bool implementsParams() const noexcept override;
    bool isValidParamId(clap_id paramId) const noexcept override;
    uint32_t paramsCount() const noexcept override;
    bool paramsInfo(uint32_t paramIndex, clap_param_info *info) const noexcept override;
    bool paramsValue(clap_id paramId, double *value) noexcept override;
    bool paramsValueToText(clap_id paramId, double value, char *display, uint32_t size) noexcept override;
    bool paramsTextToValue(clap_id paramId, const char *display, double *value) noexcept override;

    JUCEParameterVariant* findVariantByParamId(clap_id param_id);

    void handleParameterChangeEvent(const clap_event_param_value *paramEvent);
    void paramSetValueAndNotifyIfChanged(JUCEParameterVariant &param, float newValue);
    void onMainThread() noexcept override;
    bool implementsLatency() const noexcept override;
    uint32_t latencyGet() const noexcept override;
    bool implementsTail() const noexcept override;
    uint32_t tailGet() const noexcept override;
    bool implementsRender() const noexcept override;
    bool renderSetMode(clap_plugin_render_mode mode) noexcept override;

    juce::MidiBuffer midiBuffer;

    clap_process_status process(const clap_process *process) noexcept override;
    void paramsFlush(const clap_input_events *in, const clap_output_events *out) noexcept override;
    void pushUIQueueToOutputEvents(const clap_output_events_t *ov);
    void process_clap_event(const clap_event_header_t *event, int sampleOffset);

    // START GUI CODE
    bool implementsGui() const noexcept override;
    bool guiIsApiSupported(const char *api, bool isFloating) noexcept override;

    struct EditorWrapperComponent : juce::Component
    {
        using HostType = clap::helpers::HostProxy<
            clap::helpers::MisbehaviourHandler::CLAP_MISBEHAVIOUR_HANDLER_LEVEL,
            clap::helpers::CheckingLevel::CLAP_CHECKING_LEVEL>;
        EditorWrapperComponent(HostType &_host, ClapJuceWrapper &_clapWrapper);
        ~EditorWrapperComponent() override;
        void createEditor(juce::AudioProcessor &plugin);
        juce::Rectangle<int> getSizeToContainChild();
        juce::Rectangle<int> convertToHostBounds(juce::Rectangle<int> pluginRect);
        void resizeHostWindow();
        void setEditorScaleFactor(float scale);
        void paint(juce::Graphics &g) override;
        void resized() override;
        void childBoundsChanged(Component *) override;

        HostType &host;
        ClapJuceWrapper &clapWrapper;
        std::unique_ptr<juce::AudioProcessorEditor> editor;
#if JUCE_VERSION >= 0x060008
        std::unique_ptr<juce::AudioProcessorEditorHostContext> editorHostContext;
#endif

    private:
        juce::Rectangle<int> lastBounds;
        bool resizingChild = false, resizingParent = false;
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EditorWrapperComponent)
    };
    std::unique_ptr<EditorWrapperComponent> editorWrapper;

    bool guiParentAttached{false};
    float guiScaleFactor = 1.0f;
    bool guiCreate(const char *api, bool isFloating) noexcept override;
    void guiDestroy() noexcept override;
    bool guiSetParent(const clap_window *window) noexcept override;
    // Show doesn't really exist in JUCE per se. If there's an editor and its attached
    // we are good.
    bool guiShow() noexcept override;
    bool guiCanResize() const noexcept override;
    bool guiSetScale(double scale) noexcept override;
    /*
     * guiAdjustSize is called before guiSetSize and given the option to
     * reset the size the host hands to the subsequent setSize. This is a
     * relatively naive and unsatisfactory initial implementation.
     */
    bool guiAdjustSize(uint32_t *w, uint32_t *h) noexcept override;
    bool guiSetSize(uint32_t width, uint32_t height) noexcept override;
    bool guiGetSize(uint32_t *width, uint32_t *height) noexcept override;
    // END GUI CODE

  protected:
    juce::CriticalSection stateInformationLock;
    juce::MemoryBlock chunkMemory;

  public:
    bool implementsState() const noexcept override;
    bool stateSave(const clap_ostream *stream) noexcept override;
    bool stateLoad(const clap_istream *stream) noexcept override;

  public:
#if JUCE_MAC
    bool guiCocoaAttach(void *nsView) noexcept;
#endif

#if JUCE_LINUX
    bool guiX11Attach(const char *displayName, unsigned long window) noexcept;
#endif

#if JUCE_WINDOWS
    bool guiWin32Attach(clap_hwnd window) const noexcept;
#endif

  private:
    struct ParamChange
    {
        int type;
        int flag;
        uint32_t id;
        float newval{0};
    };
    PushPopQ<ParamChange, 4096 * 16> uiParamChangeQ;

    struct ParamListenerCall
    {
        juce::AudioProcessorParameter *parameter = nullptr;
        float newValue = 0.0f;
    };
    PushPopQ<ParamListenerCall, 4096 * 16> audioThreadParamListenerQ;

    /*
     * Various maps for ID lookups
     */
    // clap_id to param *
    std::unordered_map<clap_id, JUCEParameterVariant> paramPtrByClapID;
    // param * to clap_id
    std::unordered_map<const juce::AudioProcessorParameter *, clap_id> clapIDByParamPtr;
    // Every id we have issued
    std::unordered_set<clap_id> allClapIDs;

    juce::LegacyAudioParametersWrapper juceParameters;

    const clap_event_transport *transportInfo{nullptr};
    bool hasTransportInfo{false};

    struct NoteNameInfo
    {
        juce::String name{};
        int16_t key = -1;
        int16_t channel = -1;
    };
    std::vector<NoteNameInfo> noteNameInfoCached{};
};

JUCE_END_IGNORE_WARNINGS_GCC_LIKE
JUCE_END_IGNORE_WARNINGS_MSVC

/*

const char *features[] = {CLAP_FEATURES, nullptr};
clap_plugin_descriptor ClapJuceWrapper::desc = {CLAP_VERSION,
                                                CLAP_ID,
                                                JucePlugin_Name,
                                                JucePlugin_Manufacturer,
                                                JucePlugin_ManufacturerWebsite,
                                                CLAP_MANUAL_URL,
                                                CLAP_SUPPORT_URL,
                                                JucePlugin_VersionString,
                                                JucePlugin_Desc,
                                                features};

JUCE_BEGIN_IGNORE_WARNINGS_GCC_LIKE("-Wredundant-decls")
juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter();
JUCE_END_IGNORE_WARNINGS_GCC_LIKE

namespace ClapAdapter
{
static bool clap_init(const char *) { return true; }

static void clap_deinit(void) {}

static uint32_t clap_get_plugin_count(const struct clap_plugin_factory *) { return 1; }

static const clap_plugin_descriptor *clap_get_plugin_descriptor(const struct clap_plugin_factory *,
                                                                uint32_t)
{
    return &ClapJuceWrapper::desc;
}

const clap_plugin *clap_create_plugin(const struct clap_plugin_factory *, const clap_host *host,
                                      const char *plugin_id)
{
    juce::ScopedJuceInitialiser_GUI libraryInitialiser;

    if (strcmp(plugin_id, ClapJuceWrapper::desc.id))
    {
        std::cout << "Warning: CLAP asked for plugin_id '" << plugin_id
                  << "' and JuceCLAPWrapper ID is '" << ClapJuceWrapper::desc.id << "'"
                  << std::endl;
        return nullptr;
    }
    clap_juce_extensions::clap_properties::building_clap = true;
    clap_juce_extensions::clap_properties::clap_version_major = CLAP_VERSION_MAJOR;
    clap_juce_extensions::clap_properties::clap_version_minor = CLAP_VERSION_MINOR;
    clap_juce_extensions::clap_properties::clap_version_revision = CLAP_VERSION_REVISION;
    clap_juce_extensions::clap_juce_audio_processor_capabilities::clapHostStatic = host;
    auto *const pluginInstance = ::createPluginFilter();
    clap_juce_extensions::clap_properties::building_clap = false;
    clap_juce_extensions::clap_juce_audio_processor_capabilities::clapHostStatic = nullptr;
    auto *wrapper = new ClapJuceWrapper(host, pluginInstance);
    return wrapper->clapPlugin();
}

static const struct clap_plugin_factory juce_clap_plugin_factory = {
    ClapAdapter::clap_get_plugin_count,
    ClapAdapter::clap_get_plugin_descriptor,
    ClapAdapter::clap_create_plugin,
};

static const void *clap_get_factory(const char *factory_id)
{
    if (strcmp(factory_id, CLAP_PLUGIN_FACTORY_ID) == 0)
    {
        return &juce_clap_plugin_factory;
    }

#if CLAP_SUPPORTS_CUSTOM_FACTORY
    return ::clapJuceExtensionCustomFactory(factory_id);
#endif

    return nullptr;
}

} // namespace ClapAdapter

*/









/*

#ifdef __cplusplus
extern "C" {
#endif

    //const __attribute__((dllexport)) clap_plugin_entry clap_entry =
    //const __declspec(dllexport) clap_plugin_entry clap_entry =
    const clap_plugin_entry clap_entry =
    {
        CLAP_VERSION,
        ClapAdapter::clap_init,
        ClapAdapter::clap_deinit,
        ClapAdapter::clap_get_factory
    };

#ifdef __cplusplus
}
#endif


extern "C"
{
#if JUCE_LINUX
#pragma GCC diagnostic ignored "-Wattributes"
#endif


#if 0
#if JUCE_MINGW
    extern
#endif
    const CLAP_EXPORT struct clap_plugin_entry clap_entry =
#endif

    const __declspec(dllexport) struct clap_plugin_entry clap_entry =
    {
        CLAP_VERSION,
        ClapAdapter::clap_init,
        ClapAdapter::clap_deinit,
        ClapAdapter::clap_get_factory
    };
}

*/