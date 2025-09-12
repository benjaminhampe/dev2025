#include <cstdint>
#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <lzma/config.h>
#include <lzma/lzmadec.h>
#include <lzma/lzma_version.h>

#ifdef WIN32
#include <fcntl.h>
#endif

#define CHUNKSIZE_IN (1024*4)
#define CHUNKSIZE_OUT (1024*512)

#include <de/Hex.hpp>
#include <de/FileSystem.hpp>

//LZMA compressed file format
//---------------------------
//Offset Size Description
//0     1   Special LZMA properties for compressed data
//1     4   Dictionary size (little endian)
//5     8   Uncompressed size (little endian). -1 means unknown size
//13         Compressed data

//#define BUFSIZE 4096

bool
find_lzma_header(uint8_t const * const buf)
{
   return (buf[0] < 0xE1
      && buf[0] == 0x5d
      && buf[4] < 0x20
      && (memcmp( buf + 10 , "\x00\x00\x00", 3) == 0
          || (memcmp( buf + 5, "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF", 8) == 0)));
}

// Extremely simple and limited logarithm function
inline uint_fast32_t
lzma_Log2 ( uint_fast32_t n )
{
   uint_fast32_t e;
   for (e = 0; n > 1; e++, n /= 2);
   return (e);
}

inline bool
lzmainfo( std::string const & uri, std::string const & argv0 )
{
   std::string fileContent = dbLoadText( uri );

   std::cout <<
   "=============================================================\n"
   "===== lzmainfo(" << uri << ") ======\n"
   "=============================================================\n"
   << de::Hex::Preview( fileContent, 10) <<
   "=============================================================\n"
   << std::endl;

   FILE *file = ::fopen( uri.c_str(), "rb" );
   if (!file)
   {
      fprintf (stderr, "%s: %s: %s\n", argv0.c_str(), uri.c_str(), strerror (errno));
      return false;
   }

   // Hack
   //uint8_t buf128[128];
   //auto nRead37 = fread(buf128, 1, 37, file);

   // Read the first bytes of the file.
   uint8_t buf[LZMADEC_MINIMUM_SIZE];
   auto nRead = fread(buf, 1, LZMADEC_MINIMUM_SIZE, file);

   lzmadec_info info;
   if (nRead != LZMADEC_MINIMUM_SIZE)
   {
      fprintf (stderr, "%s: %s: Too small to be an LZMA encoded file.\n", argv0.c_str(), uri.c_str());
      return 1; // Error
   } else if (LZMADEC_OK != lzmadec_buffer_info(&info, buf, LZMADEC_MINIMUM_SIZE))
   {
      fprintf (stderr, "%s: %s: Invalid LZMA header.\n", argv0.c_str(), uri.c_str());
      return 1;
   } else
   {
      if (file != stdin)
      {
         printf ("%s\n", uri.c_str());
      }

      printf ("Uncompressed size:             ");

      if (info.is_streamed)
         printf ("Unknown");
      else
         printf ("%llu MB (%llu bytes)", (info.uncompressed_size + 512 * 1024)/ (1024 * 1024), info.uncompressed_size);

      printf ("\nDictionary size:               "
            "%u MB (2^%u bytes)\n"
            "Literal context bits (lc):     %d\n"
            "Literal pos bits (lp):         %d\n"
            "Number of pos bits (pb):       %d\n",
            (info.dictionary_size + 512 * 1024)
            / (1024 * 1024),
            lzma_Log2(info.dictionary_size),
            (int)info.lc,
            (int)info.lp,
            (int)info.pb);
   }
   ::fclose( file );
   return 0;
}


int main(int argc, char **argv)
{
   std::cout << "[Test] #include <lzma/lzma.h>" << std::endl;

   int ret = 0;

   // Show information about files listed on the command line.
   for (int i = 1; i < argc; i++)
   {
      lzmainfo(argv[i], argv[0]);
   }

/*
   size_t write_size;
   FILE *file_in = stdin;
   FILE *file_out = stdout;
   uint8_t* buffer_in = (uint8_t*)malloc( CHUNKSIZE_IN );
   uint8_t* buffer_out = (uint8_t*)malloc( CHUNKSIZE_OUT );

   // Check the command line arguments.
   if (argc > 1 && 0 == strcmp (argv[1], "--help"))
   {
      // Breaking the indentation style
      printf (
         "\nLZMAdec - a small LZMA decoder\n\n"
         "Usage: %s [--help]\n\n"
         "The compressed data is read from stdin and uncompressed to stdout.\n\n"
         "LZMA SDK version %s - %s\n"
         "LZMA Utils version %s - %s\n"
         "\nLZMAdec is free software licensed under the GNU LGPL.\n\n",
         argv[0],
         LZMA_SDK_VERSION_STRING,
         LZMA_SDK_COPYRIGHT_STRING,
         LZMA_UTILS_VERSION_STRING,
         LZMA_UTILS_COPYRIGHT_STRING);
      return 0;
   }

#ifdef WIN32
   setmode(fileno(stdout), O_BINARY);
   setmode(fileno(stdin), O_BINARY);
#endif

   if (buffer_in == NULL || buffer_out == NULL)
   {
      fprintf (stderr, "%s: Not enough memory.\n", argv[0]);
      return 5;
   }

   // Other initializations
   lzmadec_stream strm;
   strm.lzma_alloc = NULL;
   strm.lzma_free = NULL;
   strm.opaque = NULL;
   strm.avail_in = 0;
   strm.next_in = NULL;
   if (lzmadec_init(&strm) != LZMADEC_OK)
   {
      fprintf (stderr, "Not enough memory.\n");
      return 2;
   }

   // Decode
   while (1)
   {
      if (strm.avail_in == 0)
      {
         strm.next_in = buffer_in;
         strm.avail_in = fread( buffer_in, 1, CHUNKSIZE_IN, file_in );
      }
      strm.next_out = buffer_out;
      strm.avail_out = CHUNKSIZE_OUT;
      ret = lzmadec_decode( &strm, strm.avail_in == 0 );
      if ( ret != LZMADEC_OK &&
           ret != LZMADEC_STREAM_END )
      {
         return 1;
      }
      write_size = CHUNKSIZE_OUT - strm.avail_out;
      if ( write_size != fwrite( buffer_out, 1, write_size, file_out ) )
      {
         return 2;
      }
      if (ret == LZMADEC_STREAM_END)
      {
         lzmadec_end (&strm);
         return 0;
      }
   }
*/
   return 0;
}

#if 0

#ifdef HAVE_CONFIG_H
#include <lzma/config.h>
#endif

#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#ifdef HAVE_ERRNO_H
#include <errno.h>
#else
extern int errno
#endif

#include <lzma/lzma_version.h>

// This can be compiled without liblzmadec's stdio support.
#define LZMADEC_NO_STDIO
#include <lzma/lzmadec.h>


LZMA compressed file format
---------------------------
Offset Size Description
0     1   Special LZMA properties for compressed data
1     4   Dictionary size (little endian)
5     8   Uncompressed size (little endian). -1 means unknown size
13         Compressed data

#define BUFSIZE 4096

int find_lzma_header(unsigned char *buf) {
   return (buf[0] < 0xE1
      && buf[0] == 0x5d
      && buf[4] < 0x20
      && (memcmp (buf + 10 , "\x00\x00\x00", 3) == 0
          || (memcmp (buf + 5, "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF", 8) == 0)));
}

// Extremely simple and limited logarithm function
static uint_fast32_t
Log2 (uint_fast32_t n)
{
   uint_fast32_t e;
   for (e = 0; n > 1; e++, n /= 2);
   return (e);
}

static int
lzmainfo (FILE *file, const char *name, const char *argv0)
{
   lzmadec_info info;
   uint8_t buf[LZMADEC_MINIMUM_SIZE];
   // Read the first bytes of the file.
   if (LZMADEC_MINIMUM_SIZE != fread (buf, sizeof (uint8_t),
         LZMADEC_MINIMUM_SIZE, file)) {
      fprintf (stderr, "%s: %s: Too small to be an LZMA "
            "encoded file.\n", argv0, name);
      return 1; // Error
   } else if (LZMADEC_OK != lzmadec_buffer_info (
         &info, buf, LZMADEC_MINIMUM_SIZE)) {
      fprintf (stderr, "%s: %s: Invalid LZMA header.\n",
            argv0, name);
      return 1;
   } else {
      if (file != stdin)
         printf ("%s\n", name);
      printf ("Uncompressed size:             ");
      if (info.is_streamed)
         printf ("Unknown");
      else
         printf ("%llu MB (%llu bytes)",
               (info.uncompressed_size + 512 * 1024)
               / (1024 * 1024),
               info.uncompressed_size);
      printf ("\nDictionary size:               "
            "%u MB (2^%u bytes)\n"
            "Literal context bits (lc):     %d\n"
            "Literal pos bits (lp):         %d\n"
            "Number of pos bits (pb):       %d\n",
            (info.dictionary_size + 512 * 1024)
            / (1024 * 1024),
            Log2 (info.dictionary_size),
            (int)info.lc,
            (int)info.lp,
            (int)info.pb);
   }
   fclose (file);
   return 0;
}

int
main (int argc, char **argv)
{
   FILE *file;
   int i;
   int ret = 0;

   if (argc < 2) {
      // No arguments, reading standard input.
      ret = lzmainfo (stdin, "(standard input)", argv[0]);
      return ret;
   } else if (0 == strcmp (argv[1], "--help")) {
      printf (); // Breaking the indentation style
"\nLZMAinfo - Show information stored in the LZMA file header\n\n"
"Usage: lzmainfo [--help | filename.lzma [filename2.lzma ...]]\n\n"
"If no filename is specified lzmainfo reads stdin.\n"
"The information is always printed to stdout.\n\n"
"LZMA SDK version %s - %s\n"
"LZMA utils version %s - %s\n"
"\nLZMAinfo is free software licensed under the GNU LGPL.\n\n",
         LZMA_SDK_VERSION_STRING, LZMA_SDK_COPYRIGHT_STRING,
         LZMA_UTILS_VERSION_STRING, LZMA_UTILS_COPYRIGHT_STRING);
      return 0;
   }

   // Show information about files listed on the command line.
   printf ("\n");
   for (i = 1; i < argc; i++) {
      file = fopen (argv[i], "rb");
      if (file == NULL) {
         fprintf (stderr, "%s: %s: %s\n", argv[0], argv[i],
               strerror (errno));
         continue;
      }
      ret |= lzmainfo (file, argv[i], argv[0]);
      printf ("\n");
   }
   return ret;
}

int main(int argc, char *argv[]) {
   char buf[BUFSIZE];
   int ret, i, numlzma, blocks=0;

   if (argc != 2) {
      printf("usage: %s numlzma < infile | lzma -c -d > outfile\n"
             "where numlzma is index of lzma file to extract, starting from zero.\n",
             argv[0]);
      exit(1);
   }
   numlzma = atoi(argv[1]);

   for (;;) {
      // Read data.
      ret = fread(buf, BUFSIZE, 1, stdin);
      if (ret != 1)
         break;

      // Scan for signature.
      for (i = 0; i<BUFSIZE-23; i++) {
         if (find_lzma_header(buf+i) && numlzma-- <= 0) {
            fwrite(buf+i, (BUFSIZE-i), 1, stdout);
            for (;;) {
               int ch;
               ch = getchar();
               if (ch == EOF)
                  exit(0);
               putchar(ch);
            }

         }
      }
      blocks++;
   }
   return 1;
}
#endif
