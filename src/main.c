#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "reciter.h"
#include "sam.h"
#include "debug.h"

#ifdef USESDL
#include <SDL.h>
#include <SDL_audio.h>
#endif

void WriteWav(char* filename, char* buffer, int bufferlength)
{
    FILE *file;
    //should be equivalent, but more hacky to do filename[0] == '-' && filename[1]==0
    if(strcmp(filename, "-") == 0) file = stdout;
    else file = fopen(filename, "wb");
    if (file == NULL) return;

    unsigned int filesize=bufferlength + 12 + 16 + 8 - 8;

    //RIFF header
    fwrite("RIFF", 4, 1,file);
    fwrite(&filesize, 4, 1, file);
    fwrite("WAVE", 4, 1, file);

    //format chunk
    fwrite("fmt ", 4, 1, file);
    unsigned int fmtlength = 16;
    fwrite(&fmtlength, 4, 1, file);
    unsigned short int format=1; // PCM
    fwrite(&format, 2, 1, file);
    unsigned short int channels=1; // mono
    fwrite(&channels, 2, 1, file);
    unsigned int samplerate = 22050;
    fwrite(&samplerate, 4, 1, file);
    fwrite(&samplerate, 4, 1, file); // bytes/second
    unsigned short int blockalign = 1;
    fwrite(&blockalign, 2, 1, file);
    unsigned short int bitspersample=8;
    fwrite(&bitspersample, 2, 1, file);

    //data chunk
    fwrite("data", 4, 1, file);
    fwrite(&bufferlength, 4, 1, file);
    fwrite(buffer, bufferlength, 1, file);

    fclose(file);
}

void PrintUsage()
{
    printf("%s\n","usage: sam [options] Word1 Word2 ....");
    printf("%s\n","options");
    printf("%s\n","    -phonetic         enters phonetic mode. (see below)");
    printf("%s\n","    -pitch number     set pitch value  (default=64)    ");
    printf("%s\n","    -speed number     set speed value  (default=72)    ");
    printf("%s\n","    -throat number    set throat value (default=128)   ");
    printf("%s\n","    -mouth number     set mouth value  (default=128)   ");
    printf("%s\n","    -wav filename     output to wav instead of libsdl  ");
    printf("%s\n","    -sing             special treatment of pitch       ");
    printf("%s\n","    -debug            print additional debug messages  ");
    printf("\n");
    printf("%s\n","     VOWELS                            VOICED CONSONANTS   ");
    printf("%s\n","IY           f(ee)t                    R        red        ");
    printf("%s\n","IH           p(i)n                     L        allow      ");
    printf("%s\n","EH           beg                       W        away       ");
    printf("%s\n","AE           Sam                       W        whale      ");
    printf("%s\n","AA           pot                       Y        you        ");
    printf("%s\n","AH           b(u)dget                  M        Sam        ");
    printf("%s\n","AO           t(al)k                    N        man        ");
    printf("%s\n","OH           cone                      NX       so(ng)     ");
    printf("%s\n","UH           book                      B        bad        ");
    printf("%s\n","UX           l(oo)t                    D        dog        ");
    printf("%s\n","ER           bird                      G        again      ");
    printf("%s\n","AX           gall(o)n                  J        judge      ");
    printf("%s\n","IX           dig(i)t                   Z        zoo        ");
    printf("%s\n","                                       ZH       plea(s)ure ");
    printf("%s\n","   DIPHTHONGS                          V        seven      ");
    printf("%s\n","EY           m(a)de                    DH       (th)en     ");
    printf("%s\n","AY           h(igh)                    ");
    printf("%s\n","OY           boy                       ");
    printf("%s\n","AW           h(ow)                     UNVOICED CONSONANTS ");
    printf("%s\n","OW           slow                      S         Sam       ");
    printf("%s\n","UW           crew                      SH        fish      ");
    printf("%s\n","                                       F         fish      ");
    printf("%s\n","                                       TH        thin      ");
    printf("%s\n"," SPECIAL PHONEMES                      P         poke      ");
    printf("%s\n","UL           sett(le) (=AXL)           T         talk      ");
    printf("%s\n","UM           astron(omy) (=AXM)        K         cake      ");
    printf("%s\n","UN           functi(on) (=AXN)         CH        speech    ");
    printf("%s\n","Q            kitt-en (glottal stop)    /H        a(h)ead   ");
}

#ifdef USESDL

int pos = 0;
void MixAudio(void *unused, Uint8 *stream, int len)
{
    int bufferpos = GetBufferLength();
    char *buffer = GetBuffer();
    int i;
    if (pos >= bufferpos) return;
    if ((bufferpos-pos) < len) len = (bufferpos-pos);
    for(i=0; i<len; i++)
    {
        stream[i] = buffer[pos];
        pos++;
    }
}


void OutputSound()
{
    int bufferpos = GetBufferLength();
    bufferpos /= 50;
    SDL_AudioSpec fmt;

    fmt.freq = 22050;
    fmt.format = AUDIO_U8;
    fmt.channels = 1;
    fmt.samples = 2048;
    fmt.callback = MixAudio;
    fmt.userdata = NULL;

    /* Open the audio device and start playing sound! */
    if ( SDL_OpenAudio(&fmt, NULL) < 0 )
    {
        printf("Unable to open audio: %s\n", SDL_GetError());
        exit(1);
    }
    SDL_PauseAudio(0);
    //SDL_Delay((bufferpos)/7);

    while (pos < bufferpos)
    {
        SDL_Delay(100);
    }

    SDL_CloseAudio();
}

#else

void OutputSound() {}

#endif

int debug = 0;

int main(int argc, char **argv)
{
    int i;
    int phonetic = 0;

    char* wavfilename = NULL;
    char input[256];

    for(i=0; i<256; i++) input[i] = 0;

    if (argc <= 1)
    {
        PrintUsage();
        return 1;
    }

    i = 1;
    while(i < argc)
    {
        if (argv[i][0] != '-')
        {
            strncat(input, argv[i], 255);
            strncat(input, " ", 255);
        } else
        {
            if (strcmp(&argv[i][1], "wav")==0)
            {
                wavfilename = argv[i+1];
                i++;
            } else
            if (strcmp(&argv[i][1], "sing")==0)
            {
                EnableSingmode();
            } else
            if (strcmp(&argv[i][1], "phonetic")==0)
            {
                phonetic = 1;
            } else
            if (strcmp(&argv[i][1], "debug")==0)
            {
                debug = 1;
            } else
            if (strcmp(&argv[i][1], "pitch")==0)
            {
                SetPitch(atoi(argv[i+1]));
                i++;
            } else
            if (strcmp(&argv[i][1], "speed")==0)
            {
                SetSpeed(atoi(argv[i+1]));
                i++;
            } else
            if (strcmp(&argv[i][1], "mouth")==0)
            {
                SetMouth(atoi(argv[i+1]));
                i++;
            } else
            if (strcmp(&argv[i][1], "throat")==0)
            {
                SetThroat(atoi(argv[i+1]));
                i++;
            } else
            {
                PrintUsage();
                return 1;
            }
        }

        i++;
    } //while

    for(i=0; input[i] != 0; i++)
        input[i] = toupper((int)input[i]);

    if (debug)
    {
        if (phonetic) printf("phonetic input: %s\n", input);
        else printf("text input: %s\n", input);
    }

    if (!phonetic)
    {
        strncat(input, "[", 255);
        if (!TextToPhonemes((unsigned char *)input)) return 1;
        if (debug)
            printf("phonetic input: %s\n", input);
    } else strncat(input, "\x9b", 255);

#ifdef USESDL
    if ( SDL_Init(SDL_INIT_AUDIO) < 0 )
    {
        printf("Unable to init SDL: %s\n", SDL_GetError());
        exit(1);
    }
    atexit(SDL_Quit);
#endif

    SetInput(input);
    if (!SAMMain())
    {
        PrintUsage();
        return 1;
    }

    if (wavfilename != NULL)
        WriteWav(wavfilename, GetBuffer(), GetBufferLength()/50);
    else
        OutputSound();


    return 0;

}
