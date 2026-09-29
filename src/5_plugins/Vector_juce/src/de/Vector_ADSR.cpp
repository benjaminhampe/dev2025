#include "Vector_ADSR.h"

namespace de {
namespace vec {

/*
float Env::nextSample()
{
    if (!m_bOK)
    {
        DE_ERROR("Not OK.")
        return 0.0f;
    }

    float A = 0.0f;

    // if (m_phase == Idle)
    // {
    //     return 0.0;
    // }
    // Attack:      ____ 1
    //             /|        y = m * x + n
    //            / |        m = dy / dx
    //           /  |dy      n = 0
    //     0 ___/___|       dy = m_noteOnVelocity
    //            dx        dx = m_attackSamples
    if (m_phase == Attack)
    {
        if (m_currentFrame < m_attackFrames)
        {
            A = m_mAttack * m_currentFrame;
            m_currentFrame++;
        }
        else
        {
            A = 1.0f;
            m_phase = Decay;
            m_currentFrame = 0;
        }
    }
    // Decay:
    //    1.0 __  dx                    y = m * x + n
    //          \                       m = dy / dx
    //           \   dy                 n = m_noteOnVelocity;
    //            \                     dy = -(m_noteOnVelocity - m_sustainLevel)
    //             \___ m_sustainLevel  dx = m_decaySamples
    //
    else if (m_phase == Decay)
    {
        if (m_currentFrame < m_decayFrames)
        {
            A = m_mDecay * m_currentFrame + 1.0f;
            m_currentFrame++;
        }
        else
        {
            A = m_cfg.SustainLevel;
            m_phase = Sustain;
            m_currentFrame = 0;
        }
    }
    // Constant Sustain Level:
    else if (m_phase == Sustain)
    {
        A = m_cfg.SustainLevel;

        if (m_bSustainPedal)
        {
            // Keep sustaining...
        }
        else
        {
            if (m_cfg.bSingleShot || m_bTriggeredNoteOff)
            {
                m_phase = Release;
                m_currentFrame = 0;
            }
        }
    }
    // Release:
    //    m_sustainLevel ___            y = m * x + n
    //                     |\           m = dy / dx
    //                     | \          n = m_sustainLevel
    //                  dy |  \        dy = -m_sustainLevel
    //                     |___\___ 0  dx = m_releaseSamples
    //                      dx
    else if (m_phase == Release)
    {
        if (m_currentFrame < m_releaseFrames)
        {
            A = m_mRelease * m_currentFrame + m_cfg.SustainLevel;
            m_currentFrame++;
        }
        else
        {
            m_phase = Idle;
        }
    }

    // Global velocity gain:
    if (m_cfg.bVeloAffectsGain)
    {
        A *= m_noteOnVelocity;

        if (m_cfg.bVeloSquaredGain)
        {
            A *= m_noteOnVelocity;
        }
    }

    m_frameCounter++;
    return std::clamp(A, 0.0f, 1.0f); // Limiter
}
*/

float Env::getNextSample()
{
    float A = 0.0f;

    switch (m_phase)
    {
        // AD:
        case Attack:
        {
            if (m_currentFrame < m_attackFrames)
            {
                A = m_mAttack * m_currentFrame;

                // New:
                if (onProgress)
                {
                    onProgress(Attack, m_attackFramesInv * m_currentFrame);
                }

                m_currentFrame++;
            }
            else
            {
                A = 1.0f;
                m_phase = Decay;
                m_currentFrame = 0;

                // New:
                if (onProgress)
                {
                    onProgress(Decay, 0.0f);
                }
            }
            break;
        }
        // DS:
        case Decay:
        {
            if (m_currentFrame < m_decayFrames)
            {
                A = 1.0f + m_mDecay * m_currentFrame;

                // New:
                if (onProgress)
                {
                    onProgress(Decay, m_decayFramesInv * m_currentFrame);
                }

                m_currentFrame++;
            }
            else
            {
                A = m_sustainLevel;
                m_phase = Sustain;
                m_currentFrame = 0;

                // New:
                if (onProgress)
                {
                    onProgress(Decay, 1.0f); // End of Decay = Sustain
                }
            }
            break;
        }
        case Sustain:
        {
            A = m_sustainLevel;

            // New:
            if (onProgress)
            {
                onProgress(Sustain, 0.0f);
            }

            if (m_bTriggeredNoteOff || m_cfg.bSingleShot)
            {
                m_phase = Release;
                m_currentFrame = 0;
                //m_releaseStart = A;
                //m_mRelease = -A / float(m_releaseFrames);
            }

            break;
        }
        case Release:
        {
            if (m_currentFrame < m_releaseFrames)
            {
                A = m_mRelease * m_currentFrame + m_sustainLevel;

                // New:
                if (onProgress)
                {
                    onProgress(Release, m_releaseFramesInv * m_currentFrame);
                }

                m_currentFrame++;
            }
            else
            {
                m_phase = Idle;

                // New:
                if (onProgress)
                {
                    onProgress(Idle, 0.0f);
                }
            }
            break;
        }

        default:
        {
            // New:
            if (onProgress)
            {
                onProgress(Idle, 0.0f);
            }
            break;
        }
    }

    // Apply velocity AFTER ADSR
    if (m_cfg.bVeloAffectsGain)
        A *= m_noteOnVelocity;

    m_lastOutput = A;
    return A;
}

// static
void Env::test()
{
    test1();
    test2();
}

// static
void Env::test1()
{
    Env env;
    env.m_baseAttackFrames = 200;
    env.m_baseDecayFrames = 300;
    env.m_baseSustainLevel = 0.75f;
    env.m_baseReleaseFrames = 500;
    env.m_cfg.bSingleShot = true;
    env.m_bOK = true;
    env.resetIdle();

    de::Image img(3000,256);
    img.fill(0xFFFFFFFF);

    int x = 20;
    int y = 28;
    int h = 200;

    // AttackPhase:
    env.noteOn(0.5f);
    int w = env.m_baseAttackFrames; // 400
    draw(env,w,img,de::Recti(x,y,2*w,h),dbRGB(255,0,0)); x += 2*w;

    // DecayPhase:
    w = env.m_baseDecayFrames; // 600
    draw(env,w,img,de::Recti(x,y,2*w,h),dbRGB(0,200,0)); x += 2*w;

    // SustainPhase:
    w = 100; // 200
    draw(env,w,img,de::Recti(x,y,2*w,h),dbRGB(255,200,0)); x += 2*w;

    // ReleasePhase:
    // env.triggerNoteOff(0.5f);
    w = env.m_baseReleaseFrames; //  1000
    draw(env,w,img,de::Recti(x,y,2*w,h),dbRGB(0,0,255)); // x += 2*w;

    //dbSaveImage(img,"Abenton_SineMachine5_Test1.bmp");
    //dbSaveImage(img,"Abenton_SineMachine5_Test1.png");
    dbSaveImage(img,"Abenton_SineMachine5_Test1.webp");
}

// static
void Env::test2()
{
    Env env;
    env.m_baseAttackFrames = 200;
    env.m_baseDecayFrames = 300;
    env.m_baseSustainLevel = 0.75f;
    env.m_baseReleaseFrames = 500;
    env.m_cfg.bSingleShot = false;
    env.m_bOK = true;
    env.resetIdle();

    de::Image img(3000,256);
    img.fill(0xFFFFFFFF);
    int x = 20;
    int y = 28;
    int h = 200;

    // AttackPhase:
    env.noteOn(0.5f);
    int w = env.m_baseAttackFrames; // 400
    draw(env,w,img,de::Recti(x,y,2*w,h),dbRGB(255,0,0)); x += 2*w;

    // DecayPhase:
    w = env.m_baseDecayFrames; // 600
    draw(env,w,img,de::Recti(x,y,2*w,h),dbRGB(0,200,0)); x += 2*w;

    // SustainPhase:
    w = 100; // 200
    draw(env,w,img,de::Recti(x,y,2*w,h),dbRGB(255,200,0)); x += 2*w;

    // ReleasePhase:
    env.noteOff(0.5f);
    w = env.m_baseReleaseFrames; //  1000
    draw(env,w,img,de::Recti(x,y,2*w,h),dbRGB(0,0,255));
    // x += 2*w;

    //dbSaveImage(img,"Abenton_SineMachine5_Test2.bmp");
    //dbSaveImage(img,"Abenton_SineMachine5_Test2.png");
    dbSaveImage(img,"Abenton_SineMachine5_Test2.webp");

}

// static
void Env::draw(Env & env, int nCalls, de::Image & img, const de::Recti& pos, uint32_t color)
{
    int dx = pos.w / nCalls;

    int x1 = pos.x;
    int y1 = pos.y + std::lroundf((1.0f - env.getNextSample()) * pos.h);
    for (int i = 0; i < nCalls; ++i)
    {
        int x2 = x1 + dx;
        int y2 = pos.y + std::lroundf((1.0f - env.getNextSample()) * pos.h);
        de::ImagePainter::drawLine(img,x1,y1,x2,y2,color,false);
        x1 = x2;
        y1 = y2;
    }
}


} // end namespace vec.
} // end namespace de.
