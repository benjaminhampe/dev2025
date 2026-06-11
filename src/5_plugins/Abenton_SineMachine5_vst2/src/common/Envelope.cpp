#include "Envelope.h"

float Envelope::nextSample()
{
    float A = 0.0f;
    m_frameCounter++;

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

    return std::clamp(A, 0.0f, 1.0f); // Limiter
}

// static
void Envelope::test()
{
    test1();
    test2();
}

// static
void Envelope::test1()
{
    EnvelopeCfg cfg;
    cfg.AttackFrames = 200;
    cfg.DecayFrames = 300;
    cfg.SustainLevel = 0.75;
    cfg.ReleaseFrames = 500;
    cfg.bSingleShot = true;

    Envelope env;
    env.init(cfg);

    de::Image img(3000,256);
    img.fill(0xFFFFFFFF);

    int x = 20;
    int y = 28;
    int h = 200;

    // AttackPhase:
    env.triggerNoteOn(0.5f);
    int w = cfg.AttackFrames; // 400
    draw(env,w,img,de::Recti(x,y,2*w,h),dbRGBA(255,0,0)); x += 2*w;

    // DecayPhase:
    w = cfg.DecayFrames; // 600
    draw(env,w,img,de::Recti(x,y,2*w,h),dbRGBA(0,200,0)); x += 2*w;

    // SustainPhase:
    w = 100; // 200
    draw(env,w,img,de::Recti(x,y,2*w,h),dbRGBA(255,200,0)); x += 2*w;

    // ReleasePhase:
    // env.triggerNoteOff(0.5f);
    w = cfg.ReleaseFrames; //  1000
    draw(env,w,img,de::Recti(x,y,2*w,h),dbRGBA(0,0,255)); x += 2*w;

    dbSaveImage(img,"Abenton_SineMachine5_Test1.bmp");
    dbSaveImage(img,"Abenton_SineMachine5_Test1.png");
    dbSaveImage(img,"Abenton_SineMachine5_Test1.webp");
}

// static
void Envelope::test2()
{
    EnvelopeCfg cfg;
    cfg.AttackFrames = 200;
    cfg.DecayFrames = 300;
    cfg.SustainLevel = 0.75;
    cfg.ReleaseFrames = 500;

    Envelope env;
    env.init(cfg);

    de::Image img(3000,256);
    img.fill(0xFFFFFFFF);
    int x = 20;
    int y = 28;
    int h = 200;

    // AttackPhase:
    env.triggerNoteOn(0.5f);
    int w = cfg.AttackFrames; // 400
    draw(env,w,img,de::Recti(x,y,2*w,h),dbRGBA(255,0,0)); x += 2*w;

    // DecayPhase:
    w = cfg.DecayFrames; // 600
    draw(env,w,img,de::Recti(x,y,2*w,h),dbRGBA(0,200,0)); x += 2*w;

    // SustainPhase:
    w = 100; // 200
    draw(env,w,img,de::Recti(x,y,2*w,h),dbRGBA(255,200,0)); x += 2*w;

    // ReleasePhase:
    env.triggerNoteOff(0.5f);
    w = cfg.ReleaseFrames; //  1000
    draw(env,w,img,de::Recti(x,y,2*w,h),dbRGBA(0,0,255)); x += 2*w;

    dbSaveImage(img,"Abenton_SineMachine5_Test2.bmp");
    dbSaveImage(img,"Abenton_SineMachine5_Test2.png");
    dbSaveImage(img,"Abenton_SineMachine5_Test2.webp");


}

// static
void Envelope::draw(Envelope & env, int nCalls, de::Image & img, const de::Recti& pos, uint32_t color)
{
    int dx = pos.w / nCalls;

    int x1 = pos.x;
    int y1 = pos.y + std::lroundf((1.0f - env.nextSample()) * pos.h);
    for (int i = 0; i < nCalls; ++i)
    {
        int x2 = x1 + dx;
        int y2 = pos.y + std::lroundf((1.0f - env.nextSample()) * pos.h);
        de::ImagePainter::drawLine(img,x1,y1,x2,y2,color,false);
        x1 = x2;
        y1 = y2;
    }
}
