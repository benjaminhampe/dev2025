#include <cstdlib>
#include <sstream>
#include <iostream>
// #include <DarkAsyncSpawn.h>
#include <thread>

std::string CC = "signalsmith_pitchtime_cmd.exe";

#if 0

all: out/stretch

// out/stretch: main.cpp ../signalsmith-stretch.h util/*.h util/*.hxx
    mkdir -p out
    g++ -std=c++11 -O3 -g \
        -Wall -Wextra -Wfatal-errors -Wpedantic -pedantic-errors \
        -framework Accelerate -DSIGNALSMITH_USE_ACCELERATE \
        -I ../include \
        main.cpp -o out/stretch

run-all.sh
    for name in
        matrix-and-futurebound
        sines
        dno-solo
        drums
        piano-space
        funky
        two-steps-from-hell;
    do
        "$@" "--write-preset=${PREFIX}preset.txt" "$BASEDIR/$name.wav" "$PREFIX$name.wav"
#endif

/*
# Uses input files from: https://signalsmith-audio.co.uk/code/stretch/inputs.zip
examples: out/stretch
    mkdir -p out/examples
    inputs/run-all.sh out/examples/u2- out/stretch --semitones=2
    inputs/run-all.sh out/examples/d2- out/stretch --semitones=-2
    inputs/run-all.sh out/examples/u4- out/stretch --semitones=4
    inputs/run-all.sh out/examples/d4- out/stretch --semitones=-4
    inputs/run-all.sh out/examples/u8- out/stretch --semitones=8
    inputs/run-all.sh out/examples/d8- out/stretch --semitones=-8
    inputs/run-all.sh out/examples/u16- out/stretch --semitones=16
    inputs/run-all.sh out/examples/d16- out/stretch --semitones=-16
    inputs/run-all.sh out/examples/t_8- out/stretch --time=0.8
    inputs/run-all.sh out/examples/t1_2- out/stretch --time=1.2
    inputs/run-all.sh out/examples/t1_5- out/stretch --time=1.5
    inputs/run-all.sh out/examples/t2- out/stretch --time=2
    inputs/run-all.sh out/examples/t4- out/stretch --time=4

TEST_WAV ?= "inputs/voice.wav"

dev: out/stretch
    out/stretch --time=0.8 --semitones=10 $(TEST_WAV) out/shift.wav
    out/stretch --time=0.8 --semitones=10 --formant-comp $(TEST_WAV) out/shift-fc.wav
    out/stretch --time=0.8 --semitones=10 --formant-comp --formant=3 $(TEST_WAV) out/shift-fc-f3.wav
    out/stretch --time=0.8 --semitones=10 --formant-comp --formant=3 --formant-base=500 $(TEST_WAV) out/shift-fc-f3-fb500.wav
    out/stretch --time=0.8 --semitones=10 --formant-comp --formant=2 --formant-base=100 $(TEST_WAV) out/shift-fc-f2-fb100.wav

clean:
    rm -rf out

### Example use of CMake

cmake:
    # CMAKE_BUILD_TYPE is needed for single-config generators (e.g. Makefiles)
    cmake -B out/build -DCMAKE_BUILD_TYPE=Release
    cmake --build out/build --config Release




# Use: run-all.sh out/prefix [command]
# Will call [command] input.wav out/prefix-output.wav

BASEDIR=$(dirname $0)
PREFIX=$1
shift

if [ "$#" -ge 1 ]
then
    for name in matrix-and-futurebound sines dno-solo drums piano-space funky two-steps-from-hell;
    do
        "$@" "--write-preset=${PREFIX}preset.txt" "$BASEDIR/$name.wav" "$PREFIX$name.wav"
    done
else
    echo "Not enough arguments"
    echo
    echo "Usage:"
    echo "	run-all.sh out/prefix cmd..."
    echo
    echo 'Calls "cmd... name.wav out/prefix-name.wav" for each input'
fi



int test(int testIndex, double time, int semitones, int formant, int formant_base, std::string src, std::string dstBase)
{
    std::ostringstream d;
    d << dstBase;

    std::ostringstream o;
    o << CC;

    if (time>0.0)
    {
        o << " --time=" << time;
        d << "_time" << time;
    }
    if (semitones!=0)
    {
        o << " --semitones=" << semitones;
        d << "_semi" << semitones;
    }
    if (formant>0)
    {
        o << " --formant-comp";
        d << "_fc";
        if (formant>1)
        {
            o << " --formant=" << formant;
            d << "_f" << formant;
            if (formant_base>0)
            {
                o << " --formant_base=" << formant_base;
                d << "_fb" << formant_base;
            }
        }
    }

    //o << " --write-preset=" << d.str() << "_preset.txt";
    o << " " << src;
    o << " " << d.str() << ".wav";

    std::string cmd = o.str();
    std::cout << "TEST["<<testIndex<<"] " << cmd << std::endl;
    return system(cmd.c_str());
}
*/
int phaseVocoder(int testIndex, double time, double freq, int formant, int formant_base, std::string src, std::string dstBase)
{
    std::ostringstream d;
    d << dstBase;

    std::ostringstream o;
    o << CC << " phase-vocoder";

    if (time>0.0)
    {
        o << " --time=" << time;
        d << "_time" << time;
    }
    if (freq>0.0)
    {
        o << " --freq=" << freq;
        d << "_freq" << freq;
    }
    if (formant>0)
    {
        o << " --formant-comp";
        d << "_fc";
        if (formant>1)
        {
            o << " --formant=" << formant;
            d << "_f" << formant;
            if (formant_base>0)
            {
                o << " --formant_base=" << formant_base;
                d << "_fb" << formant_base;
            }
        }
    }

    o << " --write-preset=" << d.str() << "_preset.txt";
    o << " " << src;
    o << " " << d.str() << ".wav";

    std::string cmd = o.str();
    std::cout << "TEST["<<testIndex<<"] " << cmd << std::endl;

    return system(cmd.c_str());

    // std::thread([cmd,testIndex]{
    //     int exitCode = system(cmd.c_str());
    //     fprintf(stdout, "Finished TEST[%d] with %d\n", testIndex, exitCode);
    //     fflush(stdout);
    // }).detach();

    // de::AsyncSpawn::run(cmd,
    //     [testIndex](int exitCode){
    //         printf("Finished TEST[%d] with %d\n", testIndex, exitCode);
    //     }
    // );
    return 0;
}

int main(int argc, char **argv)
{
    std::string src = "media/signalsmith/piano-space.wav";
    std::string dst = "signalsmith_pitchtime_piano-space";

    //$ ./signalsmith_pitchtime_cmd.exe wsola --time=2.0 --freq=0.5 media/signalsmith/piano-space.wav signalsmith_pitchtime_piano-space_time0.8_semi10.wav
    //$ ./signalsmith_pitchtime_cmd.exe phase-vocoder --time=16.0 --freq=1 media/signalsmith/piano-space.wav signalsmith_pitchtime_8.wav

    // $ ./signalsmith_pitchtime_cmd.exe phase-vocoder --time=2.0 --freq=0.5 media/signalsmith/piano-space.wav signalsmith_pitchtime_1.wav
    // $ ./signalsmith_pitchtime_cmd.exe phase-vocoder --time=2.0 --freq=0.75 media/signalsmith/piano-space.wav signalsmith_pitchtime_2.wav
    // $ ./signalsmith_pitchtime_cmd.exe phase-vocoder --time=2.0 --freq=2 media/signalsmith/piano-space.wav signalsmith_pitchtime_3.wav
    // $ ./signalsmith_pitchtime_cmd.exe phase-vocoder --time=2.0 --freq=1 media/signalsmith/piano-space.wav signalsmith_pitchtime_4.wav
    // $ ./signalsmith_pitchtime_cmd.exe phase-vocoder --time=4.0 --freq=1 media/signalsmith/piano-space.wav signalsmith_pitchtime_5.wav
    // $ ./signalsmith_pitchtime_cmd.exe phase-vocoder --time=8.0 --freq=0.8 media/signalsmith/piano-space.wav signalsmith_pitchtime_6.wav
    // $ ./signalsmith_pitchtime_cmd.exe phase-vocoder --time=16.0 --freq=0.7 media/signalsmith/piano-space.wav signalsmith_pitchtime_7.wav
    // $ ./signalsmith_pitchtime_cmd.exe phase-vocoder --time=16.0 --freq=1 media/signalsmith/piano-space.wav signalsmith_pitchtime_8.wav
    // $ ./signalsmith_pitchtime_cmd.exe phase-vocoder --time=24.0 --freq=0.75 media/signalsmith/piano-space.wav signalsmith_pitchtime_9.wav

    // out/stretch --time=0.8 --semitones=10 $(TEST_WAV) out/shift.wav
    // out/stretch --time=0.8 --semitones=10 --formant-comp $(TEST_WAV) out/shift-fc.wav
    // out/stretch --time=0.8 --semitones=10 --formant-comp --formant=3 $(TEST_WAV) out/shift-fc-f3.wav
    // out/stretch --time=0.8 --semitones=10 --formant-comp --formant=3 --formant-base=500 $(TEST_WAV) out/shift-fc-f3-fb500.wav
    // out/stretch --time=0.8 --semitones=10 --formant-comp --formant=2 --formant-base=100 $(TEST_WAV) out/shift-fc-f2-fb100.wav

    int k = 0;
    phaseVocoder(k,2.0, 1.0, 0,0,src,dst); k++;
    phaseVocoder(k,12.0,0.8, 0,0,src,dst); k++;
    phaseVocoder(k,24.0,1.0, 0,0,src,dst); k++;
    phaseVocoder(k,2.0, 0.5, 0,0,src,dst); k++;
    phaseVocoder(k,2.0, 0.75,1,0,src,dst); k++;
    phaseVocoder(k,2.0, 2.0, 3,0,src,dst); k++;
    phaseVocoder(k,24.0,1.0, 3,500,src,dst); k++;
    phaseVocoder(k,2.0, 0.5, 2,100,src,dst); k++;

    return 0;
}
