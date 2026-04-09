#include "VST3_Plugin.h"

#ifdef BENNI_USE_VST3

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

#include <memory>
#include <string>
#include <vector>
#include <memory>
#include <cassert>

#include "base/source/fobject.h"
#include "pluginterfaces/base/ustring.h"
#include "pluginterfaces/base/funknown.h"
#include "pluginterfaces/base/ipluginbase.h"
#include "pluginterfaces/vst/ivstcomponent.h"
#include "pluginterfaces/vst/ivstaudioprocessor.h"
#include "pluginterfaces/vst/ivsteditcontroller.h"
#include "pluginterfaces/vst/ivsthostapplication.h"
#include "pluginterfaces/gui/iplugview.h"

#include "public.sdk/source/vst/hosting/module.h"
//#include "public.sdk/source/vst/hosting/factory.h"
#include "public.sdk/source/vst/hosting/hostclasses.h"
#include "public.sdk/source/vst/hosting/eventlist.h"
#include "public.sdk/source/vst/hosting/processdata.h"

// namespace Vst3 = Steinberg::Vst;
// using namespace VST3::Hosting;
// using Steinberg::FUnknown;
// using Steinberg::FUID;
// using Steinberg::kResultOk;
// using Steinberg::tresult;


namespace de {
namespace audio {

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


class VST3_Plugin_Impl
{
    struct MidiEvent
    {
        int32_t sampleOffset;
        uint8_t status;
        uint8_t data1;
        uint8_t data2;
    };

    bool m_bIsPluginOpen;
    //using Module = Hosting::Module;
    //using FactoryInfo = Steinberg::PFactoryInfo;
    //using ClassInfo = Steinberg::Vst::Hosting::ClassInfo;

    int32_t m_sampleRate = 44100;
    int32_t m_blockSize  = 512;

    // VST3::Hosting::Module::Ptr                       m_module;
    // Steinberg::IPtr<Steinberg::IPluginFactory>       m_factory;
    // Steinberg::IPtr<Steinberg::Vst::IComponent>      m_component;
    // Steinberg::IPtr<Steinberg::Vst::IEditController> m_controller;
    // Steinberg::IPtr<Steinberg::Vst::IAudioProcessor> m_audioProcessor;
    // Steinberg::IPtr<Steinberg::IPlugView>            m_view;

    VST3::Hosting::Module::Ptr                       m_module;
    //Steinberg::Vst::IPluginFactory*                m_factory;
    Steinberg::IPtr<Steinberg::IPluginFactory>       m_factory;
    Steinberg::IPtr<Steinberg::Vst::IComponent>      m_component;
    Steinberg::IPtr<Steinberg::Vst::IAudioProcessor> m_audioProcessor;
    Steinberg::IPtr<Steinberg::Vst::IEditController> m_controller;
    Steinberg::IPtr<Steinberg::Vst::IHostApplication> m_hostApp;

    // Steinberg::IPtr<SimpleHostApplication> m_hostApp;


public:
    VST3_Plugin_Impl() // const std::wstring& path
        : m_bIsPluginOpen{ false }
    {
        m_hostApp = Steinberg::IPtr<HostApp>(new HostApp());
    }
    ~VST3_Plugin_Impl()
    {
        // unload();
    }

    bool isPluginOpen() const
    {
        return m_bIsPluginOpen;
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

    void openPlugin( std::string uri )
    {
        // closePlugin();
        std::string errorDesc;
        m_module = VST3::Hosting::Module::create(uri,errorDesc);
        if (!m_module)
        {
            DE_ERROR("No module")
            return;
        }

        auto factory = m_module->getFactory();
        // if (!m_factory)
        //     return;

        if (!createComponentAndController())
            return;

        if (!initializePlugin())
            return;

/*
        m_audioProcessor = Steinberg::IPtr<Steinberg::Vst::IAudioProcessor>(m_component);
        if (!m_audioProcessor)
            return;

        Steinberg::Vst::ProcessSetup setup {};
        setup.processMode = Steinberg::Vst::kRealtime;
        setup.symbolicSampleSize = Steinberg::Vst::kSample32;
        setup.maxSamplesPerBlock = m_blockSize;
        setup.sampleRate = m_sampleRate;

#if 0
m_component->initialize(m_hostApp);
m_component->setActive(true);

Steinberg::Vst::ProcessSetup setup {};
setup.processMode = Steinberg::Vst::kRealtime;
setup.symbolicSampleSize = Steinberg::Vst::kSample32;
setup.maxSamplesPerBlock = m_blockSize;
setup.sampleRate = m_sampleRate;

m_audioProcessor->setupProcessing(setup);

// Optional:
m_audioProcessor->setProcessing(true);
#endif

        if (m_audioProcessor->setupProcessing(setup) != Steinberg::kResultOk)
            return;

        if (m_component->setActive(true) != Steinberg::kResultOk)

        // if (m_audioProcessor->setActive(true) != Steinberg::kResultOk)
        //     return;

        activateBuses();
*/
        m_bIsPluginOpen = true;
    }

    bool initializePlugin()
    {
        //m_hostApp = Steinberg::Vst::Hosting::HostApplication::create("MinimalHost");
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

    // ------------------------------------------------------------------------
    // Component + controller creation
    // ------------------------------------------------------------------------

    bool createComponentAndController()
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

    void openEditor(HWND parent);
    void closeEditor();
    bool getEditorSize(int& width, int& height);

};

VST3_Plugin::VST3_Plugin(const std::wstring& path)

VST3_Plugin::~VST3_Plugin()
{
    if (plugView)
    {
        plugView->removed();
        plugView = nullptr;
    }

    if (controller)
    {
        controller->terminate();
        controller = nullptr;
    }

    if (component)
    {
        component->terminate();
        component = nullptr;
    }

    module.reset();
}

void VST3_Plugin::openEditor(HWND parent)
{
    if (!plugView)
        return;

    // Attach to parent window
    plugView->setFrame(nullptr); // optional, for resize notifications
    plugView->attached((void*)parent, "HWND");
}

void VST3_Plugin::closeEditor()
{
    if (plugView)
        plugView->removed();
}

bool VST3_Plugin::getEditorSize(int& width, int& height)
{
    if (!plugView)
        return false;

    ViewRect r;
    if (plugView->getSize(&r) != kResultOk)
        return false;

    width = r.right - r.left;
    height = r.bottom - r.top;
    return true;
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