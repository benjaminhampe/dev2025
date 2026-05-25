#pragma once
// #include <cstdint>
#include <unordered_map>
#include <QKeyEvent>

struct Keyboard2MidiNote_Mapping
{
    std::unordered_map<int,int> m_map;

    int get(int qtKey) const
    {
        auto it = m_map.find(qtKey);
        if (it == m_map.end())
        {
            return -1;
        }
        else
        {
            return it->second;
        }
    }

    void add(int qtKey, int midiNote)
    {
        m_map[qtKey] = midiNote;
    }

    void addGermanLayout()
    {
        int k = 12+59;
        add( Qt::Key_1, 71 );


        k = 72;
        add( Qt::Key_Q, k ); k++; // C
        add( Qt::Key_2, k ); k++;
        add( Qt::Key_W, k ); k++; // D
        add( Qt::Key_3, k ); k++;
        add( Qt::Key_E, k ); k++; // E
        add( Qt::Key_R, k ); k++; // F
        add( Qt::Key_5, k ); k++;
        add( Qt::Key_T, k ); k++; // G
        add( Qt::Key_6, k ); k++;
        add( Qt::Key_Z, k ); k++; // A
        add( Qt::Key_7, k ); k++;
        add( Qt::Key_U, k ); k++; // H

        add( Qt::Key_I, k ); k++; // C
        add( Qt::Key_9, k ); k++;
        add( Qt::Key_O, k ); k++; // D
        add( Qt::Key_0, k ); k++;
        add( Qt::Key_P, k ); k++; // E
        add( Qt::Key_Udiaeresis, k ); k++; // F
        // add( Qt::Key_ssharp, k ); k++;
        add( Qt::Key_acute, k ); k++;
        add( Qt::Key_Plus, k ); k++; // G
        k++;
        add( Qt::Key_Backspace, k ); k++; // A

        k = 48;
        add( Qt::Key_Greater, k );
        add( Qt::Key_Less, k ); k++;
        add( Qt::Key_A, k ); k++;
        add( Qt::Key_Y, k ); k++;
        add( Qt::Key_S, k ); k++;
        add( Qt::Key_X, k ); k++;
        add( Qt::Key_C, k ); k++;
        add( Qt::Key_F, k ); k++;
        add( Qt::Key_V, k ); k++;
        add( Qt::Key_G, k ); k++;
        add( Qt::Key_B, k ); k++;
        add( Qt::Key_H, k ); k++;
        add( Qt::Key_N, k ); k++;
        add( Qt::Key_M, k ); k++;
        add( Qt::Key_K, k ); k++;
        add( Qt::Key_Comma, k ); k++;
        add( Qt::Key_L, k ); k++;
        add( Qt::Key_Period, k ); k++;
        add( Qt::Key_Minus, k ); k++; // F
        add( Qt::Key_Odiaeresis, k ); k++; // Ö = F#
        add( Qt::Key_Adiaeresis, k ); k++; // Ä = F#
        add( Qt::Key_NumberSign, k ); k++; // # = G#
        add( Qt::Key_Return, k ); k++; // # = A
    }
};
