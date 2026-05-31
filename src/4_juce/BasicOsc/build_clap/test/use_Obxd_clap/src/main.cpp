#include <juce_audio_plugin_client/CLAP/clap-juce-entry.h>
#include <cstdio>

int main(int argc, char** argv )
{
    fprintf( stdout, "clap version %d.%d.%d\n",
            clap_entry.clap_version.major,
clap_entry.clap_version.minor,
clap_entry.clap_version.revision);
    fflush( stdout );
    return 0;
}
