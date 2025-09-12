#pragma once
#include "Knob.h"
#include "../audio/Manta.h"
#include "../audio/Filter.h"
#include "SpectroBeamComp.h"
#include "EQPad.h"
#include "FilterResponseGraph.h"
#include "MantaComp.h"
#include "../audio/XenManager.h"

namespace gui
{
    struct LowLevel :
        public Comp
    {
        static constexpr int NumLanes = audio::Manta::NumLanes;
		static constexpr int ParamsPerLane = audio::Manta::NumParametersPerLane;
        static constexpr int NumParamsUsedInEQPad = 6;

        LowLevel(Utils& u) :
            Comp(u, "", CursorType::Default),
            eqPad(u, "Left-Click/Drag to select filters. Scroll to change slope. Right-Click to en/disable filters."),
            spectroBeam(u, u.audioProcessor.spectroBeam),
            filterResponseGraph(u),
			manta(u, eqPad.onSelectionChanged),
            
            filterParams()
        {
           addAndMakeVisible(spectroBeam);
			
            for (auto& f : filterParams)
                f = 0.f;

            addAndMakeVisible(filterResponseGraph);
            filterResponseGraph.shallUpdate = [&]()
            {
                bool needsUpdate = false;

                for (auto l = 0; l < NumLanes; ++l)
                {
                    auto offset = l * ParamsPerLane;

                    const auto nEnabled = utils.getParam(PID::Lane1Enabled, offset)->getValMod();
                    const auto nPitch = utils.getParam(PID::Lane1Pitch, offset)->getValModDenorm();
                    const auto nResonance = utils.getParam(PID::Lane1Resonance, offset)->getValModDenorm();
                    const auto nSlope = std::round(utils.getParam(PID::Lane1Slope, offset)->getValModDenorm());
                    const auto nGain = audio::decibelToGain(utils.getParam(PID::Lane1Gain, offset)->getValModDenorm());
					const auto nPitchSnap = utils.getParam(PID::Lane1PitchSnap, offset)->getValMod();

                    offset = l * NumParamsUsedInEQPad;
                    const auto i0 = offset;
                    const auto i1 = 1 + offset;
                    const auto i2 = 2 + offset;
                    const auto i3 = 3 + offset;
                    const auto i4 = 4 + offset;
					const auto i5 = 5 + offset;

                    if (filterParams[i0] != nEnabled
                        || filterParams[i1] != nPitch
                        || filterParams[i2] != nResonance
                        || filterParams[i3] != nSlope
                        || filterParams[i4] != nGain
						|| filterParams[i5] != nPitchSnap)
                    {
                        filterParams[i0] = nEnabled;
                        filterParams[i1] = nPitch;
                        filterParams[i2] = nResonance;
                        filterParams[i3] = nSlope;
                        filterParams[i4] = nGain;
						filterParams[i5] = nPitchSnap;

                        needsUpdate = true;
                    }
                }

                return needsUpdate;
            };
            filterResponseGraph.update = [&](Path& responseCurve, float w, float h)
            {
                const auto Fs = static_cast<float>(utils.audioProcessor.getSampleRate());
                const auto fsInv = 1.f / Fs;
				
                auto& xen = utils.audioProcessor.xenManager;

                audio::FilterBandpassSlope<4> fltr;
                responseCurve.clear();
                responseCurve.startNewSubPath(0.f, h);
                for (auto x = 0.f; x < w; ++x)
                {
                    const auto f = x / w;

                    const auto pitch = f * 128.f;
                    const auto freqHz = xen.noteToFreqHzWithWrap(pitch);
                    const auto rIdx = freqHz * fsInv;

                    auto mag = 0.f;
                    for (auto l = 0; l < NumLanes; ++l)
                    {
                        const auto offset = l * NumParamsUsedInEQPad;
                        const auto i0 = offset;
                        const auto i1 = 1 + offset;
                        const auto i2 = 2 + offset;
                        const auto i3 = 3 + offset;
                        const auto i4 = 4 + offset;
						const auto i5 = 5 + offset;

                        const auto nEnabled = filterParams[i0];
                        auto nPitch = filterParams[i1]; 
                        const auto nQ = filterParams[i2];
                        const auto nSlope = filterParams[i3];
                        const auto nGain = filterParams[i4];
						const auto nPitchSnap = filterParams[i5];

                        if (nPitchSnap > .5f)
							nPitch = std::round(nPitch);
						
						const auto nFc = xen.noteToFreqHzWithWrap(nPitch) * fsInv;
                        const auto g = std::round(nEnabled) * nGain;

                        fltr.setStage(static_cast<int>(nSlope));
                        fltr.setFc(nFc, nQ);
                        mag += std::abs(fltr.response(rIdx)) * g;
                    }

                    const auto y = h - h * juce::jlimit(0.f, 1.f, std::isnan(mag) ? 0.f : mag);

                    responseCurve.lineTo(x, y);
                }
                responseCurve.lineTo(w, h);
            };
			
            addAndMakeVisible(eqPad);

			eqPad.addNode(PID::Lane1Pitch, PID::Lane1Gain, PID::Lane1Resonance, PID::Lane1Enabled, { PID::Lane1Slope, PID::Lane1DelayOct, PID::Lane1DelaySemi, PID::Lane1Feedback, PID::Lane1Heat, PID::Lane1RMOct, PID::Lane1RMSemi, PID::Lane1RMDepth, PID::Lane1PitchSnap });
			eqPad.addNode(PID::Lane2Pitch, PID::Lane2Gain, PID::Lane2Resonance, PID::Lane2Enabled, { PID::Lane2Slope, PID::Lane2DelayOct, PID::Lane2DelaySemi, PID::Lane2Feedback, PID::Lane2Heat, PID::Lane2RMOct, PID::Lane2RMSemi, PID::Lane2RMDepth, PID::Lane2PitchSnap });
			eqPad.addNode(PID::Lane3Pitch, PID::Lane3Gain, PID::Lane3Resonance, PID::Lane3Enabled, { PID::Lane3Slope, PID::Lane3DelayOct, PID::Lane3DelaySemi, PID::Lane3Feedback, PID::Lane3Heat, PID::Lane3RMOct, PID::Lane3RMSemi, PID::Lane3RMDepth, PID::Lane3PitchSnap });

            addChildComponent(manta);

            layout.init
            (
                { 1, 13, 1 },
                { 1, 21, 13, 1 }
            );

            eqPad.selectAll();
        }

        void paint(Graphics&) override
        {
            
        }

        void resized() override
        {
            layout.resized();
			
			layout.place(eqPad, 1, 1, 1, 1, false);
            auto eqPadBounds = eqPad.bounds.toNearestInt();
            eqPadBounds.setX(eqPadBounds.getX() + eqPad.getX());
			eqPadBounds.setY(eqPadBounds.getY() + eqPad.getY());
            filterResponseGraph.setBounds(eqPadBounds);
            spectroBeam.setBounds(eqPadBounds);

            layout.place(manta, 1, 2, 1, 1, false);
        }

    protected:
        EQPad eqPad;
        SpectroBeamComp<11> spectroBeam;
        FilterResponseGraph2 filterResponseGraph;
        MantaComp manta;
		
        std::array<float, NumLanes * NumParamsUsedInEQPad> filterParams;
    };
}