#include "VST3_Plugin.h"

#ifdef BENNI_USE_VST3

#include <de/audio/plugin/details/VST3_Editor.h>
#include <de/audio/plugin/details/BasePluginUtils.h>
#include <App.h>

#include "pluginterfaces/base/ustring.h"
#include "pluginterfaces/base/funknown.h"
#include "pluginterfaces/vst/ivstaudioprocessor.h"
#include "pluginterfaces/vst/ivstcomponent.h"
#include "pluginterfaces/vst/ivstevents.h"
#include "pluginterfaces/vst/vsttypes.h"
#include "pluginterfaces/vst/ivsteditcontroller.h"
#include "pluginterfaces/vst/ivsthostapplication.h"
#include "pluginterfaces/gui/iplugview.h"
#include "public.sdk/source/vst/hosting/eventlist.h"
#include "public.sdk/source/vst/hosting/module.h"
#include "public.sdk/source/vst/hosting/parameterchanges.h"
#include "public.sdk/source/common/memorystream.h" // Set state

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h> // Only for QueryPerformanceFrequency

namespace de {
namespace audio {
namespace {

constexpr u64 GUARD = 64; // 64 extra bytes for "out-of-bounds" bugs.

std::string getErrorDesc(Steinberg::tresult e)
{
    switch (e)
    {
        case Steinberg::kNoInterface: return "E_NOINTERFACE";
        case Steinberg::kResultOk: return "S_OK";
        //case Steinberg::kResultTrue: return "S_OK";
        case Steinberg::kResultFalse: return "S_FALSE";
        case Steinberg::kInvalidArgument: return "E_INVALIDARG";
        case Steinberg::kNotImplemented: return "E_NOTIMPL";
        case Steinberg::kInternalError: return "E_FAIL";
        case Steinberg::kNotInitialized: return "E_UNEXPECTED";
        case Steinberg::kOutOfMemory: return "E_OUTOFMEMORY";
        default: return "Unknown";
    }
}

//===============================
struct VST3_SampleBuffers
//===============================
{
    TAlignedVector<float> m_L;
    TAlignedVector<float> m_R;

    // Audio bus storage
    std::vector<Steinberg::Vst::AudioBusBuffers> m_iBuses;
    std::vector<Steinberg::Vst::AudioBusBuffers> m_oBuses;

    // Bus[].Channels[].Buffer
    std::vector<std::vector<TAlignedVector<float>>> m_iBuffers;
    std::vector<std::vector<TAlignedVector<float>>> m_oBuffers;

    // Bus[].Channels*
    std::vector<std::vector<float*>> m_iHeads;
    std::vector<std::vector<float*>> m_oHeads;

    void setup(Steinberg::IPtr<Steinberg::Vst::IComponent>& comp, u32 blockSize)
    {
        if (!comp) { DE_WARN("No component") return; } // Nothing todo

        m_L.resize(blockSize + GUARD);
        m_R.resize(blockSize + GUARD);

        DE_WARN("========= SETUP =========== blockSize = ", blockSize)

        // ============
        // Inputs
        // ============

        auto busTyp = Steinberg::Vst::MediaTypes::kAudio;
        auto busDir = Steinberg::Vst::kInput;

        const u32 nBusAudioIn  = comp->getBusCount(busTyp, busDir);
        u32 nChannelsIn = 0;
        m_iBuses.resize(nBusAudioIn);
        m_iBuffers.resize(nBusAudioIn);
        m_iHeads.resize(nBusAudioIn);

        DE_TRACE("BusAudioIn.Count = ", nBusAudioIn)
        for (u32 i = 0; i < nBusAudioIn; i++)
        {
            Steinberg::Vst::BusInfo info;
            auto e = comp->getBusInfo(busTyp, busDir, i, info);
            if (e != Steinberg::kResultOk)
            {
                DE_ERROR("BusAudioIn[",i,"] ", getErrorDesc(e))
            }

            nChannelsIn += info.channelCount;

            m_iBuffers[i].resize(info.channelCount);
            m_iHeads[i].resize(info.channelCount);
            for (auto & b : m_iBuffers[i])
            {
                b.resize(blockSize + GUARD);
            }
            for (u32 k = 0; k < info.channelCount; k++)
            {
                m_iHeads[i][k] = m_iBuffers[i][k].data();
            }

            m_iBuses[i].numChannels = info.channelCount;
            m_iBuses[i].silenceFlags = 0;
            m_iBuses[i].channelBuffers32 = m_iHeads[i].data();
        }
        DE_TRACE("BusAudioIn.Channels = ", nChannelsIn)

        // ============
        // Outputs
        // ============
        busDir = Steinberg::Vst::kOutput;
        const u32 nBusAudioOut = comp->getBusCount(busTyp, busDir);
        DE_TRACE("BusAudioOut.Count = ", nBusAudioOut)
        u32 nChannelsOut = 0;
        m_oBuses.resize(nBusAudioOut);
        m_oBuffers.resize(nBusAudioOut);
        m_oHeads.resize(nBusAudioOut);

        for (u32 i = 0; i < nBusAudioOut; i++)
        {
            Steinberg::Vst::BusInfo info;
            auto e = comp->getBusInfo(busTyp, busDir, i, info);
            if (e != Steinberg::kResultOk)
            {
                DE_ERROR("No OutputBusInfo[",i,"]. ", getErrorDesc(e))
            }

            nChannelsOut += info.channelCount;

            m_oBuffers[i].resize(info.channelCount);
            m_oHeads[i].resize(info.channelCount);
            for (auto & b : m_oBuffers[i])
            {
                b.resize(blockSize + GUARD);
            }
            for (u32 k = 0; k < info.channelCount; k++)
            {
                m_oHeads[i][k] = m_oBuffers[i][k].data();
            }

            m_oBuses[i].numChannels = info.channelCount;
            m_oBuses[i].silenceFlags = 0;
            m_oBuses[i].channelBuffers32 = m_oHeads[i].data();
        }

        DE_TRACE("BusAudioOut.Channels = ", nChannelsOut)
    }

    void copy1( u32 blockSize )
    {
        if (blockSize < 1)
        {
            DE_ERROR("No blockSize")
            return;
        }

        // Fill all with zeroes, just to make sure...
        for (auto & b : m_iBuffers)
        {
            for (auto & c : b)
            {
                std::fill(c.begin(), c.end(), 0.0f);
            }
        }

        const auto bytesPerChannel = u64(blockSize) * sizeof(f32);

        // Copy L+R to vst3 buffers, if any...
        u32 n = 0;
        for (auto & b : m_iBuffers)
        {
            for (auto & c : b)
            {
                if (n == 0)
                {
                    std::memcpy(m_L.data(), c.data(), bytesPerChannel);
                    n++;
                }
                else if (n == 1)
                {
                    std::memcpy(m_R.data(), c.data(), bytesPerChannel);
                    n++;
                }
                else
                {
                    break;
                }
            }
        }
    }
};

class HostApp : public Steinberg::Vst::IHostApplication
{
public:
    Steinberg::tresult PLUGIN_API getName(Steinberg::Vst::String128 name) override
    {
        Steinberg::UString(name, 128).fromAscii("MinimalHost");
        return Steinberg::kResultOk;
    }

    Steinberg::tresult PLUGIN_API createInstance(Steinberg::TUID cid,
                                                 Steinberg::TUID iid,
                                                 void** obj) override
    {
        *obj = nullptr;
        return Steinberg::kNoInterface;
    }

    Steinberg::tresult PLUGIN_API queryInterface(const Steinberg::TUID _iid,
                                                 void** obj) override
    {
        QUERY_INTERFACE(_iid, obj, Steinberg::Vst::IHostApplication::iid,
                        IHostApplication)
        *obj = nullptr;
        return Steinberg::kNoInterface;
    }

    uint32_t PLUGIN_API addRef() override { return 1; }
    uint32_t PLUGIN_API release() override { return 1; }
};

class HostComponentHandler : public Steinberg::Vst::IComponentHandler
{
public:
    // Your host’s parameter storage
    std::unordered_map<Steinberg::Vst::ParamID, Steinberg::Vst::ParamValue> paramValues;

    std::atomic<Steinberg::uint32> m_refCount {1};

    Steinberg::uint32 PLUGIN_API addRef() override
    {
        return ++m_refCount;
    }

    Steinberg::uint32 PLUGIN_API release() override
    {
        auto r = --m_refCount;
        if (r == 0)
            delete this;
        return r;
    }

    Steinberg::tresult PLUGIN_API queryInterface(const Steinberg::TUID iid, void** obj) override
    {
        if (Steinberg::FUnknownPrivate::iidEqual(iid, Steinberg::Vst::IComponentHandler::iid))
        {
            *obj = static_cast<Steinberg::Vst::IComponentHandler*>(this);
            addRef();
            return Steinberg::kResultOk;
        }
        *obj = nullptr;
        return Steinberg::kNoInterface;
    }

    // Called when plugin UI begins editing a parameter
    Steinberg::tresult PLUGIN_API beginEdit (Steinberg::Vst::ParamID id) override
    {
        //DE_TRACE("[Host] beginEdit param ",id)
        return Steinberg::kResultOk;
    }

    // Called continuously while the parameter changes (automation or UI)
    Steinberg::tresult PLUGIN_API performEdit (Steinberg::Vst::ParamID id, Steinberg::Vst::ParamValue value) override
    {
        //DE_TRACE("[Host] performEdit param ",id," = ",value)

        // Update host-side parameter storage
        paramValues[id] = value;

        // If you have automation recording, you'd record here

        return Steinberg::kResultOk;
    }

    // Called when plugin UI ends editing a parameter
    Steinberg::tresult PLUGIN_API endEdit (Steinberg::Vst::ParamID id) override
    {
        //DE_TRACE("[Host] endEdit param ",id)
        return Steinberg::kResultOk;
    }

    // Called when plugin requests a restart (e.g. new parameters, IO changes)
    Steinberg::tresult PLUGIN_API restartComponent (int32_t flags) override
    {
        DE_TRACE("[Host] restartComponent flags = ",flags)

        if (flags & Steinberg::Vst::kParamValuesChanged)
        {
            DE_TRACE("[Host] -> Plugin says parameter values changed")
        }

        if (flags & Steinberg::Vst::kReloadComponent)
        {
            DE_TRACE("[Host] -> Plugin requests full reload")
        }

        if (flags & Steinberg::Vst::kIoChanged)
        {
            DE_TRACE("[Host] -> Plugin changed IO configuration")
        }

        return Steinberg::kResultOk;
    }
};

} // end namespace

class VST3_Plugin_Impl
{
public:

    struct MidiEvent
    {
        int32_t sampleOffset;
        uint8_t status;
        uint8_t data1;
        uint8_t data2;
    };

    bool m_bIsPluginOpen;
    bool m_bIsBypassed;
    bool m_bIsSynth;
    // bool m_bHasEditor = false;
    u32 m_numInputs;
    u32 m_numOutputs;
    u32 m_sampleRate;
    u32 m_blockSize;
    u32 m_pluginId;
    ITrack* m_track;
    VST3_Editor* m_editor;
    IDspChainElement* m_inputSignal;

    std::atomic< u64 > m_framePos;

    std::string m_uri;
    std::string m_directoryMultiByte;
    std::string m_pluginName;
    std::string m_pluginVendor;
    PluginClock m_midiClock;

    VST3_SampleBuffers m_sampleBuffers;
    NormalizedSumComputer m_normalizedSumComputer;

    // MIDI + automation
    Steinberg::Vst::EventList        m_midiEventListIn;
    Steinberg::Vst::ParameterChanges m_paramChanges;

    struct VST3_MidiEventQueue
    {
        Steinberg::Vst::EventList events;
        //std::vector<Steinberg::Vst::Event> events;

        std::unique_lock< std::mutex >
        lock() const
        {
            return std::unique_lock<std::mutex>(m_mutex);
        }
    private:
        std::mutex mutable m_mutex;
    };

    VST3_MidiEventQueue m_midiEventQueueIn;

    struct VST3_ParamChangeQueue
    {
        std::vector<std::pair<Steinberg::Vst::ParamID, double>> params;

        std::unique_lock< std::mutex >
        lock() const
        {
            return std::unique_lock<std::mutex>(m_mutex);
        }
    private:
        std::mutex mutable m_mutex;
    };

    VST3_ParamChangeQueue m_paramChangesIn;

    // VstTimeInfo m_timeInfo;
    // std::vector< VstMidiEvent > m_vstMidiEvents;
    // std::vector< char > m_vstEventBuffer;

    // struct MyVstMidi
    // {
    //     std::unique_lock< std::mutex >
    //     lock() const { return std::unique_lock<std::mutex>(m_mutex); }
    //     std::vector< VstMidiEvent > events;
    // private:
    //     std::mutex mutable m_mutex;
    // } m_midiEventQueue;

    //using Module = Hosting::Module;
    //using FactoryInfo = Steinberg::PFactoryInfo;
    //using ClassInfo = Steinberg::Vst::Hosting::ClassInfo;

    // VST3::Hosting::Module::Ptr                       m_module;
    // Steinberg::IPtr<Steinberg::IPluginFactory>       m_factory;
    // Steinberg::IPtr<Steinberg::Vst::IComponent>      m_component;
    // Steinberg::IPtr<Steinberg::Vst::IEditController> m_controller;
    // Steinberg::IPtr<Steinberg::Vst::IAudioProcessor> m_audioProcessor;

    VST3::Hosting::Module::Ptr                       m_module;
    Steinberg::IPtr<Steinberg::Vst::IComponent>      m_component;
    Steinberg::IPtr<Steinberg::Vst::IAudioProcessor> m_audioProcessor;
    Steinberg::IPtr<Steinberg::Vst::IEditController> m_editController;
    Steinberg::IPtr<Steinberg::Vst::IHostApplication> m_hostApp;
    Steinberg::IPtr<Steinberg::Vst::IComponentHandler> m_hostHandler;
    Steinberg::IPtr<Steinberg::IPlugView>            m_plugView;

public:
    VST3_Plugin_Impl() // const std::wstring& path
        : m_bIsPluginOpen{ false }
        , m_bIsBypassed{ false }
        , m_bIsSynth{ false }
        , m_numInputs{ 0 }
        , m_numOutputs{ 0 }
        , m_sampleRate{ 0 }
        , m_blockSize{ 0 }
        , m_pluginId{ 0 }
        , m_track{ nullptr }
        , m_editor{ nullptr }
        , m_inputSignal{ nullptr }
        , m_framePos{ 0 }
    {
        DE_DEBUG("")
        m_hostApp = Steinberg::IPtr<HostApp>(new HostApp());

        m_hostHandler = Steinberg::IPtr<HostComponentHandler>(new HostComponentHandler());
    }

    ~VST3_Plugin_Impl()
    {
        DE_DEBUG("")
        if (m_bIsPluginOpen)
        {
            DE_ERROR("No closePlugin() called.")
            closePlugin();
        }
    }

    bool isPluginOpen() const
    {
        return m_bIsPluginOpen;
    }

    void closePlugin()
    {
        if (!m_bIsPluginOpen)
        {
            DE_WARN("Already closed")
            return;
        }

        if (m_audioProcessor)
        {
            m_audioProcessor->setProcessing(false);
            m_audioProcessor = nullptr;
        }

        if (m_editor)
        {
            DE_TRACE("Close editor")
            //dispatcher(effEditClose, 0, 0, nullptr, 0.0f);
            m_editor->enableClosing();
            m_editor->close();
            m_editor->deleteLater();
            //delete m_editor;
            m_editor = nullptr;
        }

        if (m_plugView)
        {
            m_plugView->removed();
            m_plugView = nullptr;
        }

        if (m_editController)
        {
            m_editController->terminate();
            m_editController = nullptr;
        }

        if (m_component)
        {
            m_component->setActive(false);
            m_component->terminate();
            m_component      = nullptr;
        }

        m_hostApp.reset();
        m_hostHandler.reset();
        m_module.reset();

        m_bIsPluginOpen = false;
    }

    void dumpBusses()
    {
        using Steinberg::Vst::MediaTypes::kEvent;
        using Steinberg::Vst::MediaTypes::kAudio;
        using Steinberg::Vst::kInput;
        using Steinberg::Vst::kOutput;

        int numBusEventIn  = m_component->getBusCount(kEvent, kInput);
        int numBusEventOut = m_component->getBusCount(kEvent, kOutput);
        int numBusAudioIn  = m_component->getBusCount(kAudio, kInput);
        int numBusAudioOut = m_component->getBusCount(kAudio, kOutput);

        DE_TRACE("numBusEventIn = ", numBusEventIn)
        DE_TRACE("numBusEventOut = ", numBusEventOut)
        DE_TRACE("numBusAudioIn = ", numBusAudioIn)
        DE_TRACE("numBusAudioOut = ", numBusAudioOut)
    }

    void determineIsSynth()
    {
        using Steinberg::Vst::MediaTypes::kEvent;
        using Steinberg::Vst::MediaTypes::kAudio;
        using Steinberg::Vst::kInput;
        using Steinberg::Vst::kOutput;

        int eventIn  = m_component->getBusCount(kEvent, kInput);
        int audioIn  = m_component->getBusCount(kAudio, kInput);

        if ((audioIn == 0) && (eventIn > 0))
        {
            m_bIsSynth = true;
        }
        DE_TRACE("m_bIsSynth = ", m_bIsSynth)
    }

    void setAudioIn( bool bActive )
    {
        auto busTyp = Steinberg::Vst::MediaTypes::kAudio;
        auto busDir = Steinberg::Vst::kInput;

        const int n = m_component->getBusCount(busTyp, busDir);
        DE_TRACE("BusAudioIn = ", n)
        for (int i = 0; i < n; i++)
        {
            auto e = m_component->activateBus(busTyp, busDir, i, bActive);
            if (e != Steinberg::kResultOk)
            {
                DE_ERROR("BusAudioIn[",i,"] = ", getErrorDesc(e))
            }
            else
            {
                DE_TRACE("BusAudioIn[",i,"] = ", bActive ? "Active" : "Deactive")
            }
        }
    }

    void setAudioOut( bool bActive )
    {
        auto busTyp = Steinberg::Vst::MediaTypes::kAudio;
        auto busDir = Steinberg::Vst::kOutput;

        const int n = m_component->getBusCount(busTyp, busDir);
        DE_TRACE("BusAudioOut = ", n)
        for (int i = 0; i < n; i++)
        {
            auto e = m_component->activateBus(busTyp, busDir, i, true);
            if (e != Steinberg::kResultOk)
            {
                DE_ERROR("BusAudioOut[",i,"] = ", getErrorDesc(e))
            }
            else
            {
                DE_TRACE("BusAudioOut[",i,"] = ", bActive ? "Active" : "Deactive")
            }
        }
    }

    void setEventIn( bool bActive )
    {
        auto busTyp = Steinberg::Vst::MediaTypes::kEvent;
        auto busDir = Steinberg::Vst::kInput;

        const int n = m_component->getBusCount(busTyp, busDir);
        DE_TRACE("BusEventIn = ", n)
        for (int i = 0; i < n; i++)
        {
            auto e = m_component->activateBus(busTyp, busDir, i, bActive);
            if (e != Steinberg::kResultOk)
            {
                DE_ERROR("BusEventIn[",i,"] = ", getErrorDesc(e))
            }
            else
            {
                DE_TRACE("BusEventIn[",i,"] = ", bActive ? "Active" : "Deactive")
            }
        }
    }

    void setEventOut( bool bActive )
    {
        auto busTyp = Steinberg::Vst::MediaTypes::kEvent;
        auto busDir = Steinberg::Vst::kOutput;

        const int n = m_component->getBusCount(busTyp, busDir);
        DE_TRACE("BusEventOut = ", n)
        for (int i = 0; i < n; i++)
        {
            auto e = m_component->activateBus(busTyp, busDir, i, bActive);
            if (e != Steinberg::kResultOk)
            {
                DE_ERROR("BusEventOut[",i,"] = ", getErrorDesc(e))
            }
            else
            {
                DE_TRACE("BusEventOut[",i,"] = ", bActive ? "Active" : "Deactive")
            }
        }
    }

    void openPlugin( std::string uri )
    {
        using VST3::Hosting::Module;
        using VST3::Hosting::PluginFactory;
        using VST3::Hosting::ClassInfo;
        using Steinberg::Vst::IComponent;
        using Steinberg::Vst::IEditController;
        using Steinberg::MemoryStream;
        using Steinberg::Vst::MediaTypes::kEvent;
        using Steinberg::Vst::MediaTypes::kAudio;
        using Steinberg::Vst::kInput;
        using Steinberg::Vst::kOutput;
        using Steinberg::Vst::IConnectionPoint;

        if (m_bIsPluginOpen)
        {
            DE_ERROR("Already open")
            return;
        }

        if (!m_hostApp)
        {
            DE_ERROR("No hostApp")
            return;
        }

        // 0.
        std::string errorDesc;
        m_module = Module::create(uri,errorDesc);
        if (!m_module)
        {
            DE_ERROR("No module")
            return;
        }

        m_uri = uri;
        m_pluginName = dbFileBase(uri);
        m_pluginVendor = "";

        // 0.1.
        const PluginFactory& factory = m_module->getFactory();

        // 0.2 Read factory info
        VST3::Hosting::FactoryInfo factoryInfo = factory.info();
        m_pluginVendor = factoryInfo.vendor();

        DE_TRACE("Plugin.Name: ",m_pluginName)
        DE_TRACE("Plugin.Vendor: ",m_pluginVendor)
        DE_TRACE("Plugin.URL: ",factoryInfo.url())
        DE_TRACE("Plugin.Email: ",factoryInfo.email())
        DE_TRACE("Plugin.Flags: ",factoryInfo.flags())

        // 0.3 Read class IDs
        Steinberg::TUID processorCID {};
        Steinberg::TUID controllerCID {};
        bool foundProcessor  = false;
        bool foundController = false;

        int32_t cc = factory.classCount();
        DE_TRACE("Class.Count = ",cc)

        size_t i = 0;
        for (ClassInfo const& ci : factory.classInfos())
        {
            DE_TRACE("ClassInfo[",i,"].ID = ",ci.ID().toString())
            DE_TRACE("ClassInfo[",i,"].Name = ",ci.name())
            DE_DEBUG("ClassInfo[",i,"].Category = ",ci.category())
            DE_TRACE("ClassInfo[",i,"].SubCategories = ",ci.subCategoriesString())
            DE_TRACE("ClassInfo[",i,"].ClassFlags = ",dbHex(ci.classFlags()))
            DE_TRACE("ClassInfo[",i,"].Cardinality = ",ci.cardinality())
            DE_TRACE("ClassInfo[",i,"].sdkVersion = ",ci.sdkVersion())
            DE_TRACE("ClassInfo[",i,"].Vendor = ",ci.vendor())

            if (ci.category() == kVstAudioEffectClass)
            {
                DE_TRACE("ClassInfo[",i,"] Found kVstAudioEffectClass")
                Steinberg::copyTUID(processorCID, ci.ID().data());
                foundProcessor = true;
            }
            else if (ci.category() == kVstComponentControllerClass)
            {
                DE_TRACE("ClassInfo[",i,"] Found kVstComponentControllerClass")
                Steinberg::copyTUID(controllerCID, ci.ID().data());
                foundController = true;
            }

            i++;
        }

        if (!foundProcessor)
        {
            DE_ERROR("No component found!")
            return;
        }

        // I.A. Create component
        m_component = factory.createInstance<IComponent>(processorCID);
        if (!m_component)
        {
            DE_ERROR("No component.")
            return;
        }

        // I.B. Create edit controller
        if (foundController)
        {
            m_editController = factory.createInstance<IEditController>(controllerCID);
            if (!m_editController)
            {
                DE_ERROR("No m_editController")
                return;
            }
        }

        // II.A. Initialize component
        Steinberg::tresult e;
        e = m_component->initialize(m_hostApp);
        if (e != Steinberg::kResultOk)
        {
            DE_ERROR("No m_component initialized. ", getErrorDesc(e))
            return;
        }

        // II.B Initialize edit controller
        if (m_editController)
        {
            // II. Initialize edit controller
            e = m_editController->initialize(m_hostApp);
            if (e != Steinberg::kResultOk)
            {
                DE_ERROR("No m_editController->initialize(m_hostApp). ", getErrorDesc(e))
                return;
            }
        }

        // II.C DSP-State wiederherstellen (oder leer)
        MemoryStream componentStateStream;
        componentStateStream.setSize(0);
        m_component->setState(&componentStateStream);

        // III. Activate MIDI event processing:
        determineIsSynth();

        m_sampleBuffers.setup(m_component, 2048);
        setAudioIn( true );
        setAudioOut( true );
        setEventIn( true );
        setEventOut( false );

        // IV: Activate component
        #if 0
        e = m_component->setActive(true);
        if (e != Steinberg::kResultOk)
        {
            DE_ERROR("No m_component setActive(true). ", getErrorDesc(e))
            return;
        }
        #endif

        // V. Setup edit controller
        if (m_editController)
        {
            // 2.3. Host sets the component handler (automation)
            e = m_editController->setComponentHandler(m_hostHandler);
            if (e != Steinberg::kResultOk)
            {
                DE_ERROR("No m_editController->setComponentHandler(m_hostHandler). ", getErrorDesc(e))
                return;
            }

            MemoryStream stateStream;
            stateStream.setSize(0);
            e = m_editController->setState(&stateStream);
            if (e != Steinberg::kResultOk)
            {
                DE_ERROR("No m_editController->setState(&stateStream). ", getErrorDesc(e))
                // return;
            }
        }

        // 3. Host connects processor ↔ controller
        IConnectionPoint* procCP = nullptr;
        e = m_component->queryInterface(IConnectionPoint::iid, (void**)&procCP);
        if (e != Steinberg::kResultOk)
        {
            DE_ERROR("No m_component->queryInterface(Steinberg::Vst::IConnectionPoint). ", getErrorDesc(e))
            procCP = nullptr;
        }

        IConnectionPoint* ctrlCP = nullptr;
        e = m_editController->queryInterface(IConnectionPoint::iid, (void**)&ctrlCP);
        if (e != Steinberg::kResultOk)
        {
            DE_ERROR("No m_editController->queryInterface(Steinberg::Vst::IConnectionPoint). ", getErrorDesc(e))
            ctrlCP = nullptr;
        }

        if (procCP && ctrlCP)
        {
            procCP->connect(ctrlCP);
            ctrlCP->connect(procCP);
        }
        else
        {
            DE_ERROR("")
        }

        // 5.A.
        Steinberg::Vst::IAudioProcessor* audio = nullptr;
        e = m_component->queryInterface(Steinberg::Vst::IAudioProcessor::iid, (void**)&audio);
        if (e != Steinberg::kResultOk)
        {
            DE_ERROR("No m_component->queryInterface(Steinberg::Vst::IAudioProcessor). ", getErrorDesc(e))
        }

        m_audioProcessor = audio;
        if (!m_audioProcessor)
        {
            DE_ERROR("No audioProcessor")
            return;
        }

        // 5.A.
        e = m_audioProcessor->setProcessing(false);
        if (e != Steinberg::kResultOk)
        {
            DE_ERROR("No m_audioProcessor->setProcessing(false). ", getErrorDesc(e))
        }

        // 5.B.
        auto stereo = Steinberg::Vst::SpeakerArr::kStereo;
        if (m_bIsSynth)
        {
            e = m_audioProcessor->setBusArrangements(nullptr, 0, &stereo, 1);
        }
        else
        {
            e = m_audioProcessor->setBusArrangements(&stereo, 1, &stereo, 1);
        }
        if (e != Steinberg::kResultOk)
        {
            DE_ERROR("No m_audioProcessor->setBusArrangements(Stereo). ", getErrorDesc(e))
        }


        m_sampleBuffers.setup(m_component, m_blockSize);
        setAudioIn( true );
        setAudioOut( true );
        setEventIn( true );
        setEventOut( false );

        // 5.C.
        Steinberg::Vst::ProcessSetup setup {};
        setup.processMode        = Steinberg::Vst::kRealtime;
        setup.symbolicSampleSize = Steinberg::Vst::kSample32;
        setup.maxSamplesPerBlock = m_blockSize;
        setup.sampleRate         = m_sampleRate;
        e = m_audioProcessor->setupProcessing(setup);
        if (e != Steinberg::kResultOk)
        {
            DE_ERROR("No m_audioProcessor->setupProcessing(setup). ", getErrorDesc(e))
            // return;
        }

        // 5.D.
        e = m_component->setActive(true);
        if (e != Steinberg::kResultOk)
        {
            DE_ERROR("No m_component setActive(true). ", getErrorDesc(e))
            return;
        }

        // 5.E.
        e = m_audioProcessor->setProcessing(true);
        if (e != Steinberg::kResultOk)
        {
            DE_ERROR("No m_audioProcessor->setProcessing(true). ", getErrorDesc(e))
            // return;
        }

        // X. Open editor
        if (m_editController)
        {
            Steinberg::IPlugView* plugView = m_editController->createView(Steinberg::Vst::ViewType::kEditor);
            if (plugView)
            {
                DE_OK("Got IPlugView")
                m_editor = new VST3_Editor(plugView, nullptr);
            }
            else
            {
                DE_ERROR("No IPlugView")
            }
        }

        m_uri = uri;
        m_bIsPluginOpen = true;
    }

    PluginEditorWindow* getEditor()
    {
        return m_editor;
    }

    void setInputSignal( IDspChainElement* input, int i = 0 )
    {
        m_inputSignal = input;
    }

    void clearInputSignals()
    {
        m_inputSignal = nullptr;
    }

    void dsp_init(u64 frames, u32 channels, u32 sampleRate)
    {
        (void)channels;

        bool bNeedUpdate = false;
        bool bNewBufferSize = false;

        if ( m_blockSize != frames )
        {
            m_blockSize = frames;
            bNewBufferSize = true;
            bNeedUpdate = true;
        }

        if ( m_sampleRate != sampleRate )
        {
            m_sampleRate = sampleRate;
            // bNewSampleRate = true;
            bNeedUpdate = true;
        }

        if ( bNeedUpdate )
        {
            DE_WARN("frames(",frames,"), channels(",channels,"), sampleRate(",sampleRate,")")

            auto e = m_component->setActive(false);
            if (e != Steinberg::kResultOk)
            {
                DE_ERROR("No m_component->setActive(false). ", getErrorDesc(e))
            }

            // 5.E.
            e = m_audioProcessor->setProcessing(false);
            if (e != Steinberg::kResultOk)
            {
                DE_ERROR("No m_audioProcessor->setProcessing(false). ", getErrorDesc(e))
            }

            if (bNewBufferSize)
            {
                m_sampleBuffers.setup(m_component,m_blockSize);
                setAudioIn( true );
                setAudioOut( true );
                setEventIn( true );
                setEventOut( false );
            }

            Steinberg::Vst::ProcessSetup setup {};
            setup.processMode        = Steinberg::Vst::kRealtime;
            setup.symbolicSampleSize = Steinberg::Vst::kSample32;
            setup.maxSamplesPerBlock = m_blockSize;
            setup.sampleRate         = m_sampleRate;

            e = m_audioProcessor->setupProcessing(setup);
            if (e != Steinberg::kResultOk)
            {
                DE_ERROR("No m_audioProcessor->setupProcessing(setup). ", getErrorDesc(e))
            }

            e = m_component->setActive(true);
            if (e != Steinberg::kResultOk)
            {
                DE_ERROR("No m_component->setActive(true). ", getErrorDesc(e))
            }

            // 5.E.
            e = m_audioProcessor->setProcessing(true);
            if (e != Steinberg::kResultOk)
            {
                DE_ERROR("No m_audioProcessor->setProcessing(true). ", getErrorDesc(e))
            }

        }
    }

    void dsp_read(f64 pts,
                  u32 frames,
                  u32 sampleRate,
                  f32* __restrict__ outL,
                  f32* __restrict__ outR)
    {    
        if ( !outL || !outR )
        {
            throw std::runtime_error("No dst audio dsp buffer in VST2_Plugin::readSamples()!");
        }

        //===============================
        // Bypassed:
        //===============================

        if (!m_bIsPluginOpen || m_bIsBypassed)
        {
            if ( m_inputSignal )
            {
                m_inputSignal->dsp_read( pts, frames, sampleRate, outL, outR );
            }
            else
            {
                std::fill(outL,outL+frames,0.0f);
                std::fill(outR,outR+frames,0.0f);
            }

            return; // We relayed samples or filled output with zeroes
        }

        //===============================
        // Active:
        //===============================

        dsp_init(frames,2,sampleRate);

        if ( m_inputSignal )
        {
            m_inputSignal->dsp_read( pts, frames, sampleRate,
                m_sampleBuffers.m_L.data(),
                m_sampleBuffers.m_R.data() );
        }
        else
        {
            std::fill(m_sampleBuffers.m_L.begin(),
                      m_sampleBuffers.m_L.end(), 0.0f);
            std::fill(m_sampleBuffers.m_R.begin(),
                      m_sampleBuffers.m_R.end(), 0.0f);
        }

        m_sampleBuffers.copy1( frames );

        Steinberg::Vst::ProcessContext ctx{};
        ctx.state = Steinberg::Vst::ProcessContext::kTempoValid
                  | Steinberg::Vst::ProcessContext::kTimeSigValid
                  | Steinberg::Vst::ProcessContext::kProjectTimeMusicValid
                  | Steinberg::Vst::ProcessContext::kSystemTimeValid
                  | Steinberg::Vst::ProcessContext::kPlaying;

        // Required fields
        ctx.sampleRate = sampleRate;            // e.g. 48000.0
        ctx.projectTimeSamples = m_framePos;    // running sample counter
        ctx.systemTime = dbTimeInNanoseconds(); // optional but valid

        // Musical time
        ctx.tempo = 120.0;                           // BPM
        ctx.timeSigNumerator = 4;
        ctx.timeSigDenominator = 4;

        // Musical position in quarter notes
        ctx.projectTimeMusic = (double)ctx.projectTimeSamples / m_sampleRate * (ctx.tempo / 60.0);

        // Transport state
        //ctx.transportState = Steinberg::Vst::ProcessContext::kPlaying;

        // --- Audio buses ---
        Steinberg::Vst::ProcessData data {};
        data.numSamples         = m_blockSize;
        data.processMode        = Steinberg::Vst::kRealtime;
        data.symbolicSampleSize = Steinberg::Vst::kSample32;
        data.numInputs  = int(m_sampleBuffers.m_iBuses.size());
        data.numOutputs = int(m_sampleBuffers.m_oBuses.size());
        data.inputs     = m_sampleBuffers.m_iBuses.data();
        data.outputs    = m_sampleBuffers.m_oBuses.data();
        data.processContext = &ctx;

        // ======================================================
        // Process MIDI messages:
        // ======================================================
        processVstMidiEvents( data );

        // ======================================================
        // Process Audio samples:
        // ======================================================
        auto e = m_audioProcessor->process(data);
        if (e != Steinberg::kResultOk)
        {
            DE_ERROR("m_audioProcessor->process(). ", getErrorDesc(e))
        }

        // ======================================================
        // Write (L+R) VST audio output back to DspChain.
        // ======================================================

        m_framePos += frames; // atomic.

        const auto bytesPerChannel = u64(frames) * sizeof(float);

        u32 n = 0;
        for (auto & b : m_sampleBuffers.m_oBuffers)
        {
            for (auto & c : b)
            {
                // Copy [L]eft channel:
                if (n == 0)
                {
                    std::memcpy(outL, c.data(), bytesPerChannel);
                    n++;
                }
                // Copy [R]ight channel:
                else if (n == 1)
                {
                    std::memcpy(outR, c.data(), bytesPerChannel);
                    n++;
                }
                else
                {
                    break;
                }
            }
        }

        // For audio-level-meter
        m_normalizedSumComputer.calc(outL, outR, frames);

        // Thank you for participating in our DspChain dear plugin.
    }

    void processVstMidiEvents( Steinberg::Vst::ProcessData & data )
    {
        m_midiClock.restart();

        m_midiEventListIn.clear();

        // BUG! Swap fails for this non POD type with internal state.
        // if ( auto l = m_midiEventQueueIn.lock() )
        // {
        //     std::swap( m_midiEventListIn, m_midiEventQueueIn.events );
        // }

        if (auto l = m_midiEventQueueIn.lock())
        {
            // Copy events, not the container
            for (int i = 0; i < m_midiEventQueueIn.events.getEventCount(); ++i)
            {
                Steinberg::Vst::Event event;
                auto e = m_midiEventQueueIn.events.getEvent(i, event);
                if (e != Steinberg::kResultOk)
                {
                    DE_ERROR("No m_midiEventQueueIn.events.getEvent(",i,"). ", getErrorDesc(e))
                }
                else
                {
                    m_midiEventListIn.addEvent(event);
                }
            }

            m_midiEventQueueIn.events.clear();
        }

        data.inputEvents  = &m_midiEventListIn;
        data.outputEvents = nullptr; // could capture plugin‑generated events

/*
        // --- Parameter changes (automation) ---

        m_paramChanges.clearQueue();

        for (const auto& pc : m_paramChanges)
        {
            auto queue = m_paramChanges.addParameterData(pc.first, nullptr);
            if (queue)
            {
                Steinberg::int32 index = 0;
                queue->addPoint(0 // sampleOffset,
                                static_cast<Steinberg::Vst::ParamValue>(pc.second),
                                index);
            }
        }

        data.inputParameterChanges  = &m_paramChanges;
        data.outputParameterChanges = nullptr;

        data.inputEvents  = nullptr;
        data.outputEvents = nullptr;
*/
        data.inputParameterChanges  = nullptr;
        data.outputParameterChanges = nullptr;
    }

    void onShortMidiMessage(f64 pts, const midi::ShortMidiMessage& msg)
    {
        if ( !m_bIsSynth )
        {
            return;
        }

        // HOPEFULLY that fixes missing NoteOff events:
        // Pianos work ok without that, but monophonic synth are
        // beasts on a higher level...
        const double dt = m_midiClock.now(); // Clock is restarted every callback call.
        const int deltaFrames = std::clamp( int(dt * m_sampleRate),
                                            int(0),
                                            int(m_blockSize) - 10);
        const u8 command = msg.status & 0xF0;
        const u8 channel = msg.status & 0x0F;

        Steinberg::Vst::Event e = {};
        // int16 channel;	///< channel index in event bus
        // int16 pitch;		///< range [0, 127] = [C-2, G8] with A3=440Hz (12-TET: twelve-tone equal temperament)
        // float tuning;    ///< 1.f = +1 cent, -1.f = -1 cent
        // float velocity;	///< range [0.0, 1.0]
        // int32 length;	///< in sample frames (optional, Note Off has to follow in any case!)
        // int32 noteId;	///< note identifier (if not available then -1)

        if (command == 0x90 && msg.data2 != 0) // Note On
        {
            e.type = Steinberg::Vst::Event::kNoteOnEvent;
            e.noteOn.channel = int16_t(channel);
            e.noteOn.pitch = int16_t(msg.data1);
            e.noteOn.velocity = float(msg.data2) / 127.0f;
            e.sampleOffset = deltaFrames; // at start of block

        }
        else if (command == 0x80 || (command == 0x90 && msg.data2 == 0)) // Note Off
        {
            e.type = Steinberg::Vst::Event::kNoteOffEvent;
            e.noteOff.channel = int16_t(channel);
            e.noteOff.pitch = int16_t(msg.data1);
            e.noteOff.velocity = float(msg.data2) / 127.0f;
            e.sampleOffset = deltaFrames;
        }
        else if (command == 0xF0)
        {
            // Status	Meaning         Sent continuously?
            // F8       Timing Clock    (24 ticks per quarter note)	Yes
            // F9       Undefined       No
            // FA       Start           Once
            // FB       Continue        Once
            // FC       Stop            Once
            // FD       Undefined       No
            // FE       Active Sensing	Some devices send it every ~300ms
            // FF       System Reset	Rare
            return;
        }
        else
        {
            DE_ERROR("Unsupported midi message ", msg.str())
            return;
        }

        // Special event: All Notes Off (Bn 7B 00):
        if ((command == 0xB0) && (msg.data1 == 0x7B) && (msg.data2 == 0x00))
        {
            if ( auto l = m_midiEventQueueIn.lock() )
            {
                m_midiEventQueueIn.events.clear();
            }
            return;
        }

        if ( auto l = m_midiEventQueueIn.lock() )
        {
            m_midiEventQueueIn.events.addEvent( e );
        }

        // DE_DEBUG("events(",n,"), byte1(",dbHex(byte1),"), data1(",dbHex(data1),"), data2(",dbHex(data2),")")
    }

    void onMidiMessage(f64 pts, const midi::MidiMessage& msg)
    {
        DE_WARN("Not implemented, ", msg.size())
    }
};

// ============================================================================
VST3_Plugin::VST3_Plugin()
// ============================================================================
    : _d( new VST3_Plugin_Impl )
{
    DE_TRACE("")
}

VST3_Plugin::~VST3_Plugin()
{
    DE_TRACE("")
    App::instance()->getMidiCentral().deregisterListener(this);
    delete _d;
}

// ===================================================

void VST3_Plugin::dsp_init(u64 frames, u32 channels, u32 sampleRate)
{
    _d->dsp_init(frames, channels, sampleRate);
}

void VST3_Plugin::dsp_read(f64 pts,
                          u32 frames,
                          u32 sampleRate,
                          f32* __restrict__ L,
                          f32* __restrict__ R)
{
    _d->dsp_read(pts, frames, sampleRate, L, R);
}

u32 VST3_Plugin::dsp_getInputSignalCount() const
{
    return 1;
}

IDspChainElement* VST3_Plugin::dsp_getInputSignal(int i)
{
    return _d->m_inputSignal;
}

void VST3_Plugin::dsp_setInputSignal(IDspChainElement* inSignal, int i)
{
    _d->setInputSignal(inSignal, i);
}

void VST3_Plugin::dsp_clearInputSignals()
{
    _d->clearInputSignals();
}

bool VST3_Plugin::isBypassed() const
{
    return _d->m_bIsBypassed;
}

void VST3_Plugin::setBypassed( bool bBypassed )
{
    _d->m_bIsBypassed = bBypassed;
}
// ===================================================

const ITrack* VST3_Plugin::getTrack() const { return _d->m_track; }

ITrack* VST3_Plugin::getTrack() { return _d->m_track; }

void VST3_Plugin::setTrack(ITrack* track) { _d->m_track = track; }

// ===================================================

u32 VST3_Plugin::getPluginId() const { return _d->m_pluginId; }

void VST3_Plugin::setPluginId( u32 pluginId ) { _d->m_pluginId = pluginId; }

// ===================================================

std::string VST3_Plugin::getUri() const { return _d->m_uri; }

std::string VST3_Plugin::getName() const { return _d->m_pluginName; }

std::string VST3_Plugin::getVendor() const { return _d->m_pluginVendor; }

// ===================================================

void VST3_Plugin::openPlugin( std::string uri )
{
    _d->openPlugin( uri );
}

void VST3_Plugin::closePlugin()
{
    _d->closePlugin();
}

bool VST3_Plugin::isPluginOpen() const
{
    return _d->m_bIsPluginOpen;
}

// ===================================================

bool VST3_Plugin::isSynth() const
{
    return _d->m_bIsSynth;
}

PluginEditorWindow* VST3_Plugin::getEditor()
{
    return _d->getEditor();
}

// ===================================================

void VST3_Plugin::onMidiMessage(f64 pts, const midi::MidiMessage& msg)
{
    _d->onMidiMessage(pts, msg);
}

void VST3_Plugin::onShortMidiMessage(f64 pts, const midi::ShortMidiMessage& msg)
{
    _d->onShortMidiMessage(pts, msg);
}

// ===================================================

u32 VST3_Plugin::getProgramCount() const
{
    return 0;
}

int VST3_Plugin::getProgram() const
{
    return 0;
}

void VST3_Plugin::setProgram( int i )
{

}

// ===================================================

u32 VST3_Plugin::getParameterCount() const
{
    return 0;
}

f32 VST3_Plugin::getParameter(int i) const
{
    return 0.0f;
}

void VST3_Plugin::setParameter(int i, f32 value)
{

}





float VST3_Plugin::getSpecialValue( eSpecialValue type ) const
{
    switch (type)
    {
        case IPlugin::eSV_NormalizedSumL: return _d->m_normalizedSumComputer.m_sumL;
        case IPlugin::eSV_NormalizedSumR: return _d->m_normalizedSumComputer.m_sumR;
        default: return 0.0f;
    }
}




} // end namespace audio.
} // end namespace de.

#endif // BENNI_USE_VST3



















#if 0

#include <string>
#include <vector>
#include <memory>
#include <cstring>

#include "pluginterfaces/base/funknown.h"
#include "pluginterfaces/base/ipluginbase.h"
#include "pluginterfaces/base/ustring.h"
#include "pluginterfaces/vst/ivstcomponent.h"
#include "pluginterfaces/vst/ivstaudioprocessor.h"
#include "pluginterfaces/vst/ivsteditcontroller.h"
#include "pluginterfaces/vst/ivsthostapplication.h"
#include "pluginterfaces/vst/ivstparameterchanges.h"
#include "pluginterfaces/vst/ivstevents.h"
#include "pluginterfaces/gui/iplugview.h"

#include "public.sdk/source/vst/hosting/module.h"
#include "public.sdk/source/vst/hosting/eventlist.h"
#include "public.sdk/source/vst/hosting/processdata.h"

// ---------------- HostApp ----------------

class HostApp : public Steinberg::IHostApplication
{
public:
    Steinberg::tresult PLUGIN_API getName(Steinberg::Vst::String128 name) override
    {
        Steinberg::UString(name, 128).fromAscii("MinimalHost");
        return Steinberg::kResultOk;
    }

    Steinberg::tresult PLUGIN_API createInstance(Steinberg::TUID cid,
                                                 Steinberg::TUID iid,
                                                 void** obj) override
    {
        *obj = nullptr;
        return Steinberg::kNoInterface;
    }

    Steinberg::tresult PLUGIN_API queryInterface(const Steinberg::TUID _iid,
                                                 void** obj) override
    {
        QUERY_INTERFACE(_iid, obj, Steinberg::IHostApplication::iid, IHostApplication)
        *obj = nullptr;
        return Steinberg::kNoInterface;
    }

    uint32 PLUGIN_API addRef() override  { return 1; }
    uint32 PLUGIN_API release() override { return 1; }
};

// ---------------- Vst3Host ----------------

class Vst3Host
{
public:
    Vst3Host()
        : m_sampleRate(44100.0)
        , m_blockSize(512)
        , m_numInChannels(2)
        , m_numOutChannels(2)
        , m_initialized(false)
        , m_module(nullptr)
        , m_factory(nullptr)
    {}

    bool load(const std::string& path,
              double sampleRate,
              int32_t blockSize,
              int32_t numInChannels,
              int32_t numOutChannels)
    {
        unload();

        m_sampleRate     = sampleRate;
        m_blockSize      = blockSize;
        m_numInChannels  = numInChannels;
        m_numOutChannels = numOutChannels;

        m_module = Steinberg::Vst::Hosting::Module::create(path.c_str());
        if (!m_module)
            return false;

        m_factory = m_module->getFactory();
        if (!m_factory)
            return false;

        if (!createInstances())
            return false;

        if (!initializePlugin())
            return false;

        m_initialized = true;
        return true;
    }

    void unload()
    {
        if (!m_initialized)
        {
            m_module.reset();
            m_factory = nullptr;
            return;
        }

        if (m_audioProcessor)
            m_audioProcessor->setProcessing(false);

        if (m_component)
        {
            m_component->setActive(false);
            m_component->terminate();
        }

        if (m_controller)
            m_controller->terminate();

        m_audioProcessor = nullptr;
        m_controller     = nullptr;
        m_component      = nullptr;
        m_hostApp        = nullptr;
        m_factory        = nullptr;
        m_module.reset();

        m_initialized = false;
    }

    bool isLoaded() const
    {
        return m_initialized && m_component && m_audioProcessor;
    }

    // Reconfigure sample rate / block size / channels
    bool reconfigure(double sampleRate,
                     int32_t blockSize,
                     int32_t numInChannels,
                     int32_t numOutChannels)
    {
        if (!m_initialized)
            return false;

        m_sampleRate     = sampleRate;
        m_blockSize      = blockSize;
        m_numInChannels  = numInChannels;
        m_numOutChannels = numOutChannels;

        // Tell component that IO / processing setup changed
        m_component->setActive(false);

        Steinberg::Vst::ProcessSetup setup {};
        setup.processMode        = Steinberg::Vst::kRealtime;
        setup.symbolicSampleSize = Steinberg::Vst::kSample32;
        setup.maxSamplesPerBlock = m_blockSize;
        setup.sampleRate         = m_sampleRate;

        if (m_audioProcessor->setupProcessing(setup) != Steinberg::kResultOk)
            return false;

        m_component->setActive(true);
        return true;
    }

    // Audio + MIDI + automation
    bool process(float** inputs,
                 float** outputs,
                 int32_t numSamples,
                 const std::vector<Steinberg::Vst::Event>& midiEvents,
                 const std::vector<std::pair<Steinberg::Vst::ParamID, double>>& paramChanges)
    {
        if (!m_initialized || !m_audioProcessor)
            return false;

        Steinberg::Vst::ProcessData data {};
        data.numSamples         = numSamples;
        data.processMode        = Steinberg::Vst::kRealtime;
        data.symbolicSampleSize = Steinberg::Vst::kSample32;

        // --- Audio buses ---

        m_inBuses.clear();
        m_outBuses.clear();

        m_inBuses.resize(1);
        m_outBuses.resize(1);

        m_inChannelPtrs.clear();
        m_outChannelPtrs.clear();

        m_inChannelPtrs.resize(m_numInChannels);
        m_outChannelPtrs.resize(m_numOutChannels);

        for (int32_t c = 0; c < m_numInChannels; ++c)
            m_inChannelPtrs[c] = inputs && inputs[c] ? inputs[c] : nullptr;

        for (int32_t c = 0; c < m_numOutChannels; ++c)
            m_outChannelPtrs[c] = outputs && outputs[c] ? outputs[c] : nullptr;

        m_inBuses[0].numChannels      = m_numInChannels;
        m_inBuses[0].channelBuffers32 = m_inChannelPtrs.data();

        m_outBuses[0].numChannels      = m_numOutChannels;
        m_outBuses[0].channelBuffers32 = m_outChannelPtrs.data();

        data.numInputs  = static_cast<Steinberg::int32>(m_inBuses.size());
        data.numOutputs = static_cast<Steinberg::int32>(m_outBuses.size());
        data.inputs     = m_inBuses.data();
        data.outputs    = m_outBuses.data();

        // --- MIDI events ---

        m_eventList.clear();
        for (const auto& e : midiEvents)
            m_eventList.addEvent(e);

        data.inputEvents  = &m_eventList;
        data.outputEvents = nullptr; // could capture plugin‑generated events

        // --- Parameter changes (automation) ---

        m_paramChanges.clearQueue();

        for (const auto& pc : paramChanges)
        {
            auto queue = m_paramChanges.addParameterData(pc.first, nullptr);
            if (queue)
            {
                Steinberg::int32 index = 0;
                queue->addPoint(0 /*sampleOffset*/,
                                static_cast<Steinberg::Vst::ParamValue>(pc.second),
                                index);
            }
        }

        data.inputParameterChanges  = &m_paramChanges;
        data.outputParameterChanges = nullptr;

        return m_audioProcessor->process(data) == Steinberg::kResultOk;
    }

    // Editor creation (you still need platform glue to embed the view)
    Steinberg::IPlugView* createView()
    {
        if (!m_controller)
            return nullptr;

        return m_controller->createView(Steinberg::Vst::ViewType::kEditor);
    }

private:
    bool createInstances()
    {
        Steinberg::TUID componentCID {};
        Steinberg::TUID controllerCID {};
        bool foundComponent  = false;
        bool foundController = false;

        int32_t count = m_factory->countClasses();
        for (int32_t i = 0; i < count; ++i)
        {
            Steinberg::PClassInfo ci {};
            if (m_factory->getClassInfo(i, &ci) != Steinberg::kResultOk)
                continue;

            if (std::strcmp(ci.category, kVstAudioEffectClass) == 0)
            {
                Steinberg::copyTUID(componentCID, ci.cid);
                foundComponent = true;
            }
            else if (std::strcmp(ci.category, kVstComponentControllerClass) == 0)
            {
                Steinberg::copyTUID(controllerCID, ci.cid);
                foundController = true;
            }
        }

        if (!foundComponent)
            return false;

        if (m_factory->createInstance(componentCID,
                                      Steinberg::Vst::IComponent::iid,
                                      (void**)&m_component) != Steinberg::kResultOk)
            return false;

        if (foundController)
        {
            m_factory->createInstance(controllerCID,
                                      Steinberg::Vst::IEditController::iid,
                                      (void**)&m_controller);
        }

        m_audioProcessor = Steinberg::FCast<Steinberg::Vst::IAudioProcessor>(m_component);
        return m_audioProcessor != nullptr;
    }

    bool initializePlugin()
    {
        m_hostApp = new HostApp();

        if (m_component->initialize(m_hostApp) != Steinberg::kResultOk)
            return false;

        if (m_component->setActive(true) != Steinberg::kResultOk)
            return false;

        Steinberg::Vst::ProcessSetup setup {};
        setup.processMode        = Steinberg::Vst::kRealtime;
        setup.symbolicSampleSize = Steinberg::Vst::kSample32;
        setup.maxSamplesPerBlock = m_blockSize;
        setup.sampleRate         = m_sampleRate;

        if (m_audioProcessor->setupProcessing(setup) != Steinberg::kResultOk)
            return false;

        m_audioProcessor->setProcessing(true);
        return true;
    }

private:
    double  m_sampleRate;
    int32_t m_blockSize;
    int32_t m_numInChannels;
    int32_t m_numOutChannels;
    bool    m_initialized;

    std::shared_ptr<Steinberg::Vst::Hosting::Module> m_module;
    Steinberg::IPluginFactory*                       m_factory;

    Steinberg::IPtr<Steinberg::Vst::IComponent>      m_component;
    Steinberg::IPtr<Steinberg::Vst::IAudioProcessor> m_audioProcessor;
    Steinberg::IPtr<Steinberg::Vst::IEditController> m_controller;
    Steinberg::IPtr<Steinberg::IHostApplication>     m_hostApp;

    // Audio bus storage
    std::vector<Steinberg::Vst::AudioBusBuffers> m_inBuses;
    std::vector<Steinberg::Vst::AudioBusBuffers> m_outBuses;
    std::vector<float*>                          m_inChannelPtrs;
    std::vector<float*>                          m_outChannelPtrs;

    // MIDI + automation
    Steinberg::Vst::EventList        m_eventList;
    Steinberg::Vst::ParameterChanges m_paramChanges;
};

#endif

#if 0

#include <string>
#include <memory>
#include <cstring>

#include "pluginterfaces/base/funknown.h"
#include "pluginterfaces/base/ipluginbase.h"
#include "pluginterfaces/vst/ivstcomponent.h"
#include "pluginterfaces/vst/ivstaudioprocessor.h"
#include "pluginterfaces/vst/ivsteditcontroller.h"
#include "pluginterfaces/vst/ivsthostapplication.h"
#include "pluginterfaces/gui/iplugview.h"

#include "public.sdk/source/vst/hosting/module.h"
#include "public.sdk/source/vst/hosting/hostclasses.h"
#include "public.sdk/source/vst/hosting/processdata.h"

class Vst3Host
{
public:
    Vst3Host()
        : m_sampleRate(44100.0)
        , m_blockSize(512)
        , m_initialized(false)
        , m_module(nullptr)
        , m_factory(nullptr)
    {}

    bool load(const std::string& path, double sampleRate, int32_t blockSize)
    {
        unload();

        m_sampleRate = sampleRate;
        m_blockSize  = blockSize;

        m_module = Steinberg::Vst::Hosting::Module::create(path.c_str());
        if (!m_module)
            return false;

        m_factory = m_module->getFactory();
        if (!m_factory)
            return false;

        if (!createInstances())
            return false;

        if (!initializePlugin())
            return false;

        m_initialized = true;
        return true;
    }

    void unload()
    {
        if (!m_initialized)
        {
            m_module.reset();
            m_factory = nullptr;
            return;
        }

        if (m_audioProcessor)
            m_audioProcessor->setProcessing(false);

        if (m_component)
        {
            m_component->setActive(false);
            m_component->terminate();
        }

        if (m_controller)
            m_controller->terminate();

        m_audioProcessor = nullptr;
        m_controller     = nullptr;
        m_component      = nullptr;
        m_hostApp        = nullptr;
        m_factory        = nullptr;
        m_module.reset();

        m_initialized = false;
    }

    bool isLoaded() const
    {
        return m_initialized && m_component && m_audioProcessor;
    }

    // Minimal mono in/out processing example
    bool process(float** inputs, float** outputs, int32_t numSamples)
    {
        if (!m_initialized || !m_audioProcessor)
            return false;

        Steinberg::Vst::ProcessData data {};
        data.numSamples         = numSamples;
        data.processMode        = Steinberg::Vst::kRealtime;
        data.symbolicSampleSize = Steinberg::Vst::kSample32;

        Steinberg::Vst::AudioBusBuffers inBus {};
        Steinberg::Vst::AudioBusBuffers outBus {};

        inBus.numChannels       = 1;
        inBus.channelBuffers32  = inputs && inputs[0] ? &inputs[0] : nullptr;

        outBus.numChannels      = 1;
        outBus.channelBuffers32 = outputs && outputs[0] ? &outputs[0] : nullptr;

        data.numInputs  = 1;
        data.numOutputs = 1;
        data.inputs     = &inBus;
        data.outputs    = &outBus;

        return m_audioProcessor->process(data) == Steinberg::kResultOk;
    }

    Steinberg::IPlugView* createView()
    {
        if (!m_controller)
            return nullptr;

        return m_controller->createView(Steinberg::Vst::ViewType::kEditor);
    }

private:
    bool createInstances()
    {
        Steinberg::TUID componentCID {};
        Steinberg::TUID controllerCID {};
        bool foundComponent  = false;
        bool foundController = false;

        int32_t count = m_factory->countClasses();
        for (int32_t i = 0; i < count; ++i)
        {
            Steinberg::PClassInfo ci {};
            if (m_factory->getClassInfo(i, &ci) != Steinberg::kResultOk)
                continue;

            if (std::strcmp(ci.category, kVstAudioEffectClass) == 0)
            {
                Steinberg::copyTUID(componentCID, ci.cid);
                foundComponent = true;
            }
            else if (std::strcmp(ci.category, kVstComponentControllerClass) == 0)
            {
                Steinberg::copyTUID(controllerCID, ci.cid);
                foundController = true;
            }
        }

        if (!foundComponent)
            return false;

        if (m_factory->createInstance(componentCID,
                                      Steinberg::Vst::IComponent::iid,
                                      (void**)&m_component) != Steinberg::kResultOk)
            return false;

        if (foundController)
        {
            m_factory->createInstance(controllerCID,
                                      Steinberg::Vst::IEditController::iid,
                                      (void**)&m_controller);
        }

        m_audioProcessor = Steinberg::FCast<Steinberg::Vst::IAudioProcessor>(m_component);
        return m_audioProcessor != nullptr;
    }

    bool initializePlugin()
    {
        m_hostApp = Steinberg::Vst::Hosting::HostApplication::create("MinimalHost");
        if (!m_hostApp)
            return false;

        if (m_component->initialize(m_hostApp) != Steinberg::kResultOk)
            return false;

        if (m_component->setActive(true) != Steinberg::kResultOk)
            return false;

        Steinberg::Vst::ProcessSetup setup {};
        setup.processMode        = Steinberg::Vst::kRealtime;
        setup.symbolicSampleSize = Steinberg::Vst::kSample32;
        setup.maxSamplesPerBlock = m_blockSize;
        setup.sampleRate         = m_sampleRate;

        if (m_audioProcessor->setupProcessing(setup) != Steinberg::kResultOk)
            return false;

        m_audioProcessor->setProcessing(true);
        return true;
    }

private:
    double m_sampleRate;
    int32_t m_blockSize;
    bool m_initialized;

    std::shared_ptr<Steinberg::Vst::Hosting::Module> m_module;
    Steinberg::Vst::IPluginFactory*                  m_factory;

    Steinberg::IPtr<Steinberg::Vst::IComponent>      m_component;
    Steinberg::IPtr<Steinberg::Vst::IAudioProcessor> m_audioProcessor;
    Steinberg::IPtr<Steinberg::Vst::IEditController> m_controller;
    Steinberg::IPtr<Steinberg::Vst::IHostApplication> m_hostApp;
};

#endif

#if 0

#include <string>
#include <memory>
#include <vector>
#include <cstring>

#include "pluginterfaces/base/funknown.h"
#include "pluginterfaces/base/ipluginbase.h"
#include "pluginterfaces/vst/ivstcomponent.h"
#include "pluginterfaces/vst/ivstaudioprocessor.h"
#include "pluginterfaces/vst/ivsteditcontroller.h"
#include "pluginterfaces/vst/ivsthostapplication.h"
#include "pluginterfaces/gui/iplugview.h"

#include "public.sdk/source/vst/hosting/module.h"
#include "public.sdk/source/vst/hosting/factory.h"
#include "public.sdk/source/vst/hosting/hostclasses.h"
#include "public.sdk/source/vst/hosting/eventlist.h"
#include "public.sdk/source/vst/hosting/processdata.h"

class Vst3Host
{
public:
    Vst3Host()
        : m_sampleRate(44100.0)
        , m_blockSize(512)
        , m_initialized(false)
    {}

    bool load(const std::string& path, double sampleRate, int32_t blockSize)
    {
        unload();

        m_sampleRate = sampleRate;
        m_blockSize  = blockSize;

        m_module = Steinberg::Vst::Hosting::Module::create(path.c_str());
        if (!m_module)
            return false;

        m_factory = m_module->getFactory();
        if (!m_factory)
            return false;

        if (!createComponentAndController())
            return false;

        if (!initializeComponentAndProcessor())
            return false;

        m_initialized = true;
        return true;
    }

    void unload()
    {
        if (!m_initialized)
        {
            m_module.reset();
            m_factory = nullptr;
            return;
        }

        if (m_audioProcessor)
        {
            // Optional: stop processing if you used setProcessing(true)
            m_audioProcessor->setProcessing(false);
        }

        if (m_component)
        {
            m_component->setActive(false);
            m_component->terminate();
        }

        if (m_controller)
        {
            m_controller->terminate();
        }

        m_audioProcessor = nullptr;
        m_controller     = nullptr;
        m_component      = nullptr;
        m_factory        = nullptr;
        m_module.reset();

        m_initialized = false;
    }

    bool isLoaded() const
    {
        return m_initialized && m_component && m_audioProcessor;
    }

    // Simple audio process example: mono in/out, no events
    bool process(float** inputs, float** outputs, int32_t numSamples)
    {
        if (!m_initialized || !m_audioProcessor)
            return false;

        Steinberg::Vst::ProcessData data {};
        data.numSamples = numSamples;
        data.processMode = Steinberg::Vst::kRealtime;
        data.symbolicSampleSize = Steinberg::Vst::kSample32;

        Steinberg::Vst::AudioBusBuffers inBus {};
        Steinberg::Vst::AudioBusBuffers outBus {};

        inBus.numChannels  = 1;
        inBus.channelBuffers32 = inputs[0] ? &inputs[0] : nullptr;

        outBus.numChannels = 1;
        outBus.channelBuffers32 = outputs[0] ? &outputs[0] : nullptr;

        data.numInputs  = 1;
        data.numOutputs = 1;
        data.inputs     = &inBus;
        data.outputs    = &outBus;

        return m_audioProcessor->process(data) == Steinberg::kResultOk;
    }

    // Basic editor open/close (no platform glue here)
    Steinberg::IPlugView* createView()
    {
        if (!m_controller)
            return nullptr;

        Steinberg::IPlugView* view = m_controller->createView(Steinberg::Vst::ViewType::kEditor);
        return view;
    }

private:
    bool createComponentAndController()
    {
        Steinberg::TUID componentCID {};
        Steinberg::TUID controllerCID {};
        bool componentFound  = false;
        bool controllerFound = false;

        const auto& classes = m_factory->classInfos();
        for (const auto& ci : classes)
        {
            const auto& classInfo = ci;

            if (std::strcmp(classInfo.category(), kVstAudioEffectClass) == 0)
            {
                Steinberg::copyTUID(componentCID, classInfo.cid());
                componentFound = true;
            }
            else if (std::strcmp(classInfo.category(), kVstComponentControllerClass) == 0)
            {
                Steinberg::copyTUID(controllerCID, classInfo.cid());
                controllerFound = true;
            }
        }

        if (!componentFound)
            return false;

        if (m_factory->createInstance(componentCID, Steinberg::Vst::IComponent::iid, (void**)&m_component) != Steinberg::kResultOk)
            return false;

        // Controller is optional but usually present
        if (controllerFound)
        {
            m_factory->createInstance(controllerCID, Steinberg::Vst::IEditController::iid, (void**)&m_controller);
        }

        // Query audio processor interface from component
        if (m_component)
        {
            m_audioProcessor = Steinberg::FCast<Steinberg::Vst::IAudioProcessor>(m_component);
            if (!m_audioProcessor)
                return false;
        }

        return true;
    }

    bool initializeComponentAndProcessor()
    {
        if (!m_component || !m_audioProcessor)
            return false;

        // Minimal host application stub
        m_hostApp = Steinberg::Vst::Hosting::HostApplication::create("MinimalHost");
        if (!m_hostApp)
            return false;

        if (m_component->initialize(m_hostApp) != Steinberg::kResultOk)
            return false;

        if (m_component->setActive(true) != Steinberg::kResultOk)
            return false;

        Steinberg::Vst::ProcessSetup setup {};
        setup.processMode        = Steinberg::Vst::kRealtime;
        setup.symbolicSampleSize = Steinberg::Vst::kSample32;
        setup.maxSamplesPerBlock = m_blockSize;
        setup.sampleRate         = m_sampleRate;

        if (m_audioProcessor->setupProcessing(setup) != Steinberg::kResultOk)
            return false;

        // Optional: some plugins expect this, others ignore it
        m_audioProcessor->setProcessing(true);

        return true;
    }

private:
    double m_sampleRate;
    int32_t m_blockSize;
    bool m_initialized;

    std::shared_ptr<Steinberg::Vst::Hosting::Module>  m_module;
    std::shared_ptr<Steinberg::Vst::Hosting::Factory> m_factory;

    Steinberg::IPtr<Steinberg::Vst::IComponent>       m_component;
    Steinberg::IPtr<Steinberg::Vst::IAudioProcessor>  m_audioProcessor;
    Steinberg::IPtr<Steinberg::Vst::IEditController>  m_controller;
    Steinberg::IPtr<Steinberg::Vst::IHostApplication> m_hostApp;
};

#endif

#if 0

#include "pluginterfaces/base/funknown.h"
#include "pluginterfaces/base/ustring.h"
#include "pluginterfaces/base/ipluginbase.h"
#include "pluginterfaces/base/iplugview.h"
#include "pluginterfaces/base/futils.h"

#include "pluginterfaces/vst/ivstaudioprocessor.h"
#include "pluginterfaces/vst/ivstcomponent.h"
#include "pluginterfaces/vst/ivsteditcontroller.h"
#include "pluginterfaces/vst/ivstevents.h"
#include "pluginterfaces/vst/ivstparameterchanges.h"
#include "pluginterfaces/vst/ivsthostapplication.h"
#include "pluginterfaces/vst/ivstconnectionpoint.h"

#include "public.sdk/source/vst/hosting/module.h"
#include "public.sdk/source/vst/hosting/hostclasses.h"
#include "public.sdk/source/vst/hosting/processdata.h"

// ============================================================================
// Simple host application
// ============================================================================

class SimpleHostApplication : public Steinberg::Vst::HostApplication
{
public:
    DELEGATE_REFCOUNT(Steinberg::Vst::HostApplication)

    Steinberg::tresult PLUGIN_API queryInterface(const Steinberg::TUID iid,
                                                 void** obj) override
    {
        QUERY_INTERFACE(iid, obj,
                        Steinberg::Vst::IHostApplication::iid,
                        Steinberg::Vst::IHostApplication)

        *obj = nullptr;
        return Steinberg::kNoInterface;
    }

    Steinberg::tresult PLUGIN_API getName(Steinberg::Vst::String128 name) override
    {
        Steinberg::UString(name, 128).fromAscii("SimpleVST3Host");
        return Steinberg::kResultOk;
    }
};

// ============================================================================
// VST3 plugin wrapper
// ============================================================================

class Vst3PluginWrapper
{
public:
    struct MidiEvent
    {
        int32_t sampleOffset;
        uint8_t status;
        uint8_t data1;
        uint8_t data2;
    };

public:
    Vst3PluginWrapper()
    {
        m_hostApp = Steinberg::IPtr<SimpleHostApplication>(new SimpleHostApplication());
    }

    ~Vst3PluginWrapper()
    {
        unload();
    }

    bool load(const std::string& path,
              int32_t sampleRate,
              int32_t blockSize)
    {
        unload();

        m_sampleRate = sampleRate;
        m_blockSize  = blockSize;

        m_module = Steinberg::Vst::Hosting::Module::create(path.c_str());
        if (!m_module)
            return false;

        if (!m_module->isLoaded() && !m_module->load())
            return false;

        m_factory = m_module->getFactory();
        if (!m_factory)
            return false;

        if (!createComponentAndController())
            return false;

        m_audioProcessor = Steinberg::IPtr<Steinberg::Vst::IAudioProcessor>(m_component);
        if (!m_audioProcessor)
            return false;

        Steinberg::Vst::ProcessSetup setup {};
        setup.processMode = Steinberg::Vst::kRealtime;
        setup.symbolicSampleSize = Steinberg::Vst::kSample32;
        setup.maxSamplesPerBlock = m_blockSize;
        setup.sampleRate = m_sampleRate;

        if (m_audioProcessor->setupProcessing(setup) != Steinberg::kResultOk)
            return false;

        if (m_audioProcessor->setActive(true) != Steinberg::kResultOk)
            return false;

        activateBuses();

        m_initialized = true;
        return true;
    }

    void unload()
    {
        closeEditor();

        if (m_audioProcessor)
            m_audioProcessor->setActive(false);

        if (m_component)
            m_component->terminate();

        if (m_controller)
            m_controller->terminate();

        m_audioProcessor = nullptr;
        m_controller     = nullptr;
        m_component      = nullptr;
        m_factory        = nullptr;

        //if (m_module && m_module->isLoaded())
        //    m_module->unload();

        m_module.reset();   // Module::create() already loaded it; reset unloads it.

        m_initialized = false;
    }

    bool isLoaded() const { return m_initialized; }

    // ------------------------------------------------------------------------
    // Audio processing
    // ------------------------------------------------------------------------

    bool processAudio(float** inputs,
                      float** outputs,
                      int32_t inCh,
                      int32_t outCh,
                      int32_t numSamples)
    {
        if (!m_initialized || !m_audioProcessor)
            return false;

        Steinberg::Vst::ProcessData data {};
        data.numSamples = numSamples;
        data.symbolicSampleSize = Steinberg::Vst::kSample32;

        Steinberg::Vst::AudioBusBuffers inBus {};
        Steinberg::Vst::AudioBusBuffers outBus {};

        inBus.numChannels = inCh;
        inBus.channelBuffers32 = inputs;

        outBus.numChannels = outCh;
        outBus.channelBuffers32 = outputs;

        data.numInputs  = (inCh  > 0) ? 1 : 0;
        data.numOutputs = (outCh > 0) ? 1 : 0;
        data.inputs  = (inCh  > 0) ? &inBus  : nullptr;
        data.outputs = (outCh > 0) ? &outBus : nullptr;

        return m_audioProcessor->process(data) == Steinberg::kResultOk;
    }

    // ------------------------------------------------------------------------
    // Audio + MIDI processing
    // ------------------------------------------------------------------------

    bool processAudioMidi(float** inputs,
                          float** outputs,
                          int32_t inCh,
                          int32_t outCh,
                          int32_t numSamples,
                          const std::vector<MidiEvent>& midi)
    {
        if (!m_initialized || !m_audioProcessor)
            return false;

        Steinberg::Vst::ProcessData data {};
        data.numSamples = numSamples;
        data.symbolicSampleSize = Steinberg::Vst::kSample32;

        Steinberg::Vst::AudioBusBuffers inBus {};
        Steinberg::Vst::AudioBusBuffers outBus {};

        inBus.numChannels = inCh;
        inBus.channelBuffers32 = inputs;

        outBus.numChannels = outCh;
        outBus.channelBuffers32 = outputs;

        data.numInputs  = (inCh  > 0) ? 1 : 0;
        data.numOutputs = (outCh > 0) ? 1 : 0;
        data.inputs  = (inCh  > 0) ? &inBus  : nullptr;
        data.outputs = (outCh > 0) ? &outBus : nullptr;

        Steinberg::Vst::EventList eventList;

        for (const MidiEvent& ev : midi)
        {
            Steinberg::Vst::Event e {};
            e.sampleOffset = ev.sampleOffset;

            uint8_t status  = ev.status & 0xF0;
            uint8_t channel = ev.status & 0x0F;

            if (status == 0x90 && ev.data2 > 0)
            {
                e.type = Steinberg::Vst::Event::kNoteOnEvent;
                e.noteOn.channel  = channel;
                e.noteOn.pitch    = ev.data1;
                e.noteOn.velocity = ev.data2 / 127.0f;
            }
            else if (status == 0x80 || (status == 0x90 && ev.data2 == 0))
            {
                e.type = Steinberg::Vst::Event::kNoteOffEvent;
                e.noteOff.channel  = channel;
                e.noteOff.pitch    = ev.data1;
                e.noteOff.velocity = ev.data2 / 127.0f;
            }
            else
            {
                continue;
            }

            eventList.addEvent(e);
        }

        data.inputEvents = &eventList;

        return m_audioProcessor->process(data) == Steinberg::kResultOk;
    }

    // ------------------------------------------------------------------------
    // Parameters
    // ------------------------------------------------------------------------

    bool setParameterNormalized(Steinberg::Vst::ParamID id, double value)
    {
        if (!m_controller)
            return false;

        m_controller->setParamNormalized(id, value);
        m_controller->performEdit(id, value);
        return true;
    }

    // ------------------------------------------------------------------------
    // Editor
    // ------------------------------------------------------------------------

    // parentWindowHandle: native handle (e.g. HWND on Windows)
    // platformType: "HWND", "NSView", "X11EmbedWindowID", ...
    bool openEditor(void* parentWindowHandle, const char* platformType)
    {
        if (!m_controller)
            return false;

        if (m_view)
            return true;

        m_view = m_controller->createView(Steinberg::Vst::ViewType::kEditor);
        if (!m_view)
            return false;

        if (m_view->isPlatformTypeSupported(platformType) != Steinberg::kResultOk)
        {
            m_view = nullptr;
            return false;
        }

        if (m_view->attached(parentWindowHandle, platformType) != Steinberg::kResultOk)
        {
            m_view = nullptr;
            return false;
        }

        return true;
    }

    void closeEditor()
    {
        if (m_view)
        {
            m_view->removed();
            m_view = nullptr;
        }
    }

private:
    // ------------------------------------------------------------------------
    // Component + controller creation
    // ------------------------------------------------------------------------

    bool createComponentAndController()
    {
        if (!m_factory)
            return false;

        Steinberg::int32 count = m_factory->countClasses();
        Steinberg::PClassInfo classInfo {};

        Steinberg::TUID componentCID {};
        Steinberg::TUID controllerCID {};
        bool componentFound  = false;
        bool controllerFound = false;

        for (Steinberg::int32 i = 0; i < count; ++i)
        {
            if (m_factory->getClassInfo(i, &classInfo) != Steinberg::kResultOk)
                continue;

            if (std::strcmp(classInfo.category, Steinberg::Vst::kVstAudioEffectClass) == 0)
            {
                Steinberg::FUnknown::iidCopy(componentCID, classInfo.cid);
                componentFound = true;
            }
            else if (std::strcmp(classInfo.category, Steinberg::Vst::kVstComponentControllerClass) == 0)
            {
                Steinberg::FUnknown::iidCopy(controllerCID, classInfo.cid);
                controllerFound = true;
            }
        }

        if (!componentFound)
            return false;

        Steinberg::FUnknown* unknown = nullptr;
        if (m_factory->createInstance(componentCID,
                                      Steinberg::Vst::IComponent::iid,
                                      reinterpret_cast<void**>(&unknown)) != Steinberg::kResultOk)
        {
            return false;
        }

        m_component = Steinberg::IPtr<Steinberg::Vst::IComponent>(unknown);
        unknown->release();
        unknown = nullptr;

        if (!m_component)
            return false;

        if (m_component->initialize(m_hostApp) != Steinberg::kResultOk)
            return false;

        // Try to get controller from component
        Steinberg::Vst::IEditController* ctrl = nullptr;
        if (m_component->queryInterface(Steinberg::Vst::IEditController::iid,
                                        reinterpret_cast<void**>(&ctrl)) == Steinberg::kResultOk)
        {
            m_controller = ctrl;
            ctrl->release();
        }
        else if (controllerFound)
        {
            if (m_factory->createInstance(controllerCID,
                                          Steinberg::Vst::IEditController::iid,
                                          reinterpret_cast<void**>(&ctrl)) == Steinberg::kResultOk)
            {
                m_controller = ctrl;
                ctrl->release();
            }
        }

        if (m_controller)
        {
            if (m_controller->initialize(m_hostApp) != Steinberg::kResultOk)
                return false;

            Steinberg::Vst::IConnectionPoint* compCP = nullptr;
            Steinberg::Vst::IConnectionPoint* ctrlCP = nullptr;

            if (m_component->queryInterface(Steinberg::Vst::IConnectionPoint::iid,
                                            reinterpret_cast<void**>(&compCP)) == Steinberg::kResultOk &&
                m_controller->queryInterface(Steinberg::Vst::IConnectionPoint::iid,
                                             reinterpret_cast<void**>(&ctrlCP)) == Steinberg::kResultOk)
            {
                compCP->connect(ctrlCP);
                ctrlCP->connect(compCP);
            }

            if (compCP) compCP->release();
            if (ctrlCP) ctrlCP->release();
        }

        return true;
    }

    // ------------------------------------------------------------------------
    // Bus activation
    // ------------------------------------------------------------------------

    void activateBuses()
    {
        if (!m_component)
            return;

        Steinberg::int32 inCount = m_component->getBusCount(Steinberg::Vst::kAudio,
                                                             Steinberg::Vst::kInput);
        Steinberg::int32 outCount = m_component->getBusCount(Steinberg::Vst::kAudio,
                                                              Steinberg::Vst::kOutput);

        for (Steinberg::int32 i = 0; i < inCount; ++i)
            m_component->activateBus(Steinberg::Vst::kAudio,
                                     Steinberg::Vst::kInput, i, true);

        for (Steinberg::int32 i = 0; i < outCount; ++i)
            m_component->activateBus(Steinberg::Vst::kAudio,
                                     Steinberg::Vst::kOutput, i, true);
    }

private:
    bool m_initialized = false;

    int32_t m_sampleRate = 44100;
    int32_t m_blockSize  = 512;

    Steinberg::IPtr<SimpleHostApplication> m_hostApp;

    std::unique_ptr<Steinberg::Vst::Hosting::Module> m_module;
    Steinberg::IPtr<Steinberg::IPluginFactory>       m_factory;

    Steinberg::IPtr<Steinberg::Vst::IComponent>      m_component;
    Steinberg::IPtr<Steinberg::Vst::IEditController> m_controller;
    Steinberg::IPtr<Steinberg::Vst::IAudioProcessor> m_audioProcessor;

    Steinberg::IPtr<Steinberg::IPlugView>            m_view;

};

#endif











#if 0

    // ------------------------------------------------------------------------
    // Load plugin
    // ------------------------------------------------------------------------
    bool load(const std::string& path, int32_t sampleRate, int32_t blockSize)
    {
        unload();

        m_sampleRate = sampleRate;
        m_blockSize  = blockSize;

        // Load module
        m_module = Steinberg::Vst::Hosting::Module::create(path.c_str());
        if (!m_module)
            return false;

        if (!m_module->isLoaded() && !m_module->load())
            return false;

        // Get factory
        m_factory = m_module->getFactory();
        if (!m_factory)
            return false;

        // Create component + controller
        m_provider = std::make_unique<Steinberg::Vst::Hosting::PlugProvider>(
            m_factory, nullptr, nullptr);

        if (!m_provider->initialize())
            return false;

        m_component  = m_provider->getComponent();
        m_controller = m_provider->getController();

        if (!m_component)
            return false;

        // Query audio processor
        m_audioProcessor = Steinberg::IPtr<Steinberg::Vst::IAudioProcessor>(m_component);
        if (!m_audioProcessor)
            return false;

        // Setup processing
        Steinberg::Vst::ProcessSetup setup {};
        setup.processMode = Steinberg::Vst::kRealtime;
        setup.symbolicSampleSize = Steinberg::Vst::kSample32;
        setup.maxSamplesPerBlock = m_blockSize;
        setup.sampleRate = m_sampleRate;

        if (m_audioProcessor->setupProcessing(setup) != Steinberg::kResultOk)
            return false;

        if (m_audioProcessor->setActive(true) != Steinberg::kResultOk)
            return false;

        activateBuses();

        m_initialized = true;
        return true;
    }

    // ------------------------------------------------------------------------
    // Unload plugin
    // ------------------------------------------------------------------------
    void unload()
    {
        closeEditor();

        if (m_audioProcessor)
            m_audioProcessor->setActive(false);

        m_provider.reset();
        m_component = nullptr;
        m_controller = nullptr;
        m_audioProcessor = nullptr;
        m_factory = nullptr;

        if (m_module && m_module->isLoaded())
            m_module->unload();

        m_module.reset();
        m_initialized = false;
    }

    bool isLoaded() const { return m_initialized; }


    // ------------------------------------------------------------------------
    // Audio processing
    // ------------------------------------------------------------------------
    bool processAudio(float** inputs, float** outputs,
                      int32_t inCh, int32_t outCh, int32_t numSamples)
    {
        if (!m_initialized || !m_audioProcessor)
            return false;

        Steinberg::Vst::ProcessData data {};
        data.numSamples = numSamples;
        data.symbolicSampleSize = Steinberg::Vst::kSample32;

        Steinberg::Vst::AudioBusBuffers inBus {};
        Steinberg::Vst::AudioBusBuffers outBus {};

        inBus.numChannels = inCh;
        inBus.channelBuffers32 = inputs;

        outBus.numChannels = outCh;
        outBus.channelBuffers32 = outputs;

        data.numInputs = (inCh > 0) ? 1 : 0;
        data.numOutputs = (outCh > 0) ? 1 : 0;
        data.inputs = (inCh > 0) ? &inBus : nullptr;
        data.outputs = (outCh > 0) ? &outBus : nullptr;

        return m_audioProcessor->process(data) == Steinberg::kResultOk;
    }


    // ------------------------------------------------------------------------
    // Audio + MIDI processing
    // ------------------------------------------------------------------------
    struct MidiEvent
    {
        int32_t sampleOffset;
        uint8_t status;
        uint8_t data1;
        uint8_t data2;
    };

    bool processAudioMidi(float** inputs, float** outputs,
                          int32_t inCh, int32_t outCh,
                          int32_t numSamples,
                          const std::vector<MidiEvent>& midi)
    {
        if (!m_initialized || !m_audioProcessor)
            return false;

        Steinberg::Vst::ProcessData data {};
        data.numSamples = numSamples;
        data.symbolicSampleSize = Steinberg::Vst::kSample32;

        Steinberg::Vst::AudioBusBuffers inBus {};
        Steinberg::Vst::AudioBusBuffers outBus {};

        inBus.numChannels = inCh;
        inBus.channelBuffers32 = inputs;

        outBus.numChannels = outCh;
        outBus.channelBuffers32 = outputs;

        data.numInputs = (inCh > 0) ? 1 : 0;
        data.numOutputs = (outCh > 0) ? 1 : 0;
        data.inputs = (inCh > 0) ? &inBus : nullptr;
        data.outputs = (outCh > 0) ? &outBus : nullptr;

        // MIDI events
        Steinberg::Vst::EventList eventList;

        for (const MidiEvent& ev : midi)
        {
            Steinberg::Vst::Event e {};
            e.sampleOffset = ev.sampleOffset;

            uint8_t status = ev.status & 0xF0;
            uint8_t channel = ev.status & 0x0F;

            if (status == 0x90 && ev.data2 > 0)
            {
                e.type = Steinberg::Vst::Event::kNoteOnEvent;
                e.noteOn.channel = channel;
                e.noteOn.pitch = ev.data1;
                e.noteOn.velocity = ev.data2 / 127.0f;
            }
            else if (status == 0x80 || (status == 0x90 && ev.data2 == 0))
            {
                e.type = Steinberg::Vst::Event::kNoteOffEvent;
                e.noteOff.channel = channel;
                e.noteOff.pitch = ev.data1;
                e.noteOff.velocity = ev.data2 / 127.0f;
            }
            else
            {
                continue;
            }

            eventList.addEvent(e);
        }

        data.inputEvents = &eventList;

        return m_audioProcessor->process(data) == Steinberg::kResultOk;
    }


    // ------------------------------------------------------------------------
    // Parameter control
    // ------------------------------------------------------------------------
    bool setParameterNormalized(Steinberg::Vst::ParamID id, double value)
    {
        if (!m_controller)
            return false;

        m_controller->setParamNormalized(id, value);
        m_controller->performEdit(id, value);
        return true;
    }


    // ------------------------------------------------------------------------
    // Editor window
    // ------------------------------------------------------------------------
    bool openEditor(void* parentWindowHandle)
    {
        if (!m_controller)
            return false;

        if (m_view)
            return true; // already open

        m_view = m_controller->createView(Steinberg::Vst::ViewType::kEditor);
        if (!m_view)
            return false;

        if (m_view->isPlatformTypeSupported("HWND") != Steinberg::kResultOk)
        {
            m_view = nullptr;
            return false;
        }

        if (m_view->attached(parentWindowHandle, "HWND") != Steinberg::kResultOk)
        {
            m_view = nullptr;
            return false;
        }

        return true;
    }

    void closeEditor()
    {
        if (m_view)
        {
            m_view->removed();
            m_view = nullptr;
        }
    }


private:
    // ------------------------------------------------------------------------
    // Activate all audio buses
    // ------------------------------------------------------------------------
    void activateBuses()
    {
        int32_t inCount = m_component->getBusCount(Steinberg::Vst::kAudio,
                                                   Steinberg::Vst::kInput);
        int32_t outCount = m_component->getBusCount(Steinberg::Vst::kAudio,
                                                    Steinberg::Vst::kOutput);

        for (int32_t i = 0; i < inCount; ++i)
            m_component->activateBus(Steinberg::Vst::kAudio,
                                     Steinberg::Vst::kInput, i, true);

        for (int32_t i = 0; i < outCount; ++i)
            m_component->activateBus(Steinberg::Vst::kAudio,
                                     Steinberg::Vst::kOutput, i, true);
    }


private:
    bool m_initialized = false;

    int32_t m_sampleRate = 44100;
    int32_t m_blockSize  = 512;

    Steinberg::IPtr<SimpleHostApplication> m_hostApp;

    std::unique_ptr<Steinberg::Vst::Hosting::Module> m_module;
    Steinberg::IPtr<Steinberg::IPluginFactory> m_factory;
    std::unique_ptr<Steinberg::Vst::Hosting::PlugProvider> m_provider;

    Steinberg::IPtr<Steinberg::IPluginBase> m_component;
    Steinberg::IPtr<Steinberg::Vst::IEditController> m_controller;
    Steinberg::IPtr<Steinberg::Vst::IAudioProcessor> m_audioProcessor;

    Steinberg::IPtr<Steinberg::Vst::IPlugView> m_view;
};

#endif