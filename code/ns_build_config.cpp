/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Oskar Mendel $
   $Notice: (C) Copyright 2020 by Nullsson, Inc. All Rights Reserved. $
   ======================================================================== */

struct NSBuildConfig
{
    char *DynamicCodeFileName;
    char *EntryFileName;
    char *CompilerBackend;
    char *CompilerArguments;
    char *LinkerArguments;
};

static void
NSBuildParseConfig(char *FileContents, NSBuildConfig *Config)
{
    char *CurrentLine = FileContents;
    while(CurrentLine)
    {
        char *NextLine = strchr(CurrentLine, '\n');
        if (NextLine)
        {
            // NOTE(Oskar): Temporarily terminate the current line
            *NextLine = '\0';
        }

        if (*CurrentLine != '#')
        {
            char *Key = CurrentLine;
            char *KeyEnd = strchr(Key, '=');
            *KeyEnd = '\0';

            char *Value = KeyEnd + 1;
            unsigned int StringSize = StringLength(Value);
            char *ValueCopy = (char *)malloc(StringSize + 1);
            strcpy(ValueCopy, Value);

            if (StringsAreEqual(Key, "DYNAMICCODEFILENAME"))
            {
                Config->DynamicCodeFileName = ValueCopy;
            }
            else if (StringsAreEqual(Key, "ENTRYFILENAME"))
            {
                Config->EntryFileName = ValueCopy;
            }
            else if (StringsAreEqual(Key, "COMPILERBACKEND"))
            {
                Config->CompilerBackend = ValueCopy;
            }
            else if (StringsAreEqual(Key, "COMPILERARGUMENTS"))
            {
                Config->CompilerArguments = ValueCopy;
            }
            else if (StringsAreEqual(Key, "LINKERARGUMENTS"))
            {
                Config->LinkerArguments = ValueCopy;
            }
            else
            {
                LogError("ERROR: Found unrecognized Key in config file: %s", Key);
                free(ValueCopy);
            }
            
            *KeyEnd = '=';
        }

        if (NextLine)
        {
            // NOTE(Oskar): Restore the newline.
            *NextLine = '\n';
        }
        CurrentLine = NextLine ? (NextLine + 1) : NULL;
    }
}

static NSBuildConfig
NSBuildConfigLoad(char *ConfigPath)
{
    NSBuildConfig Config = {0};

    char *FileContents = 0;
    FILE *File = fopen(ConfigPath, "rb");
    if(File)
    {
        fseek(File, 0, SEEK_END);
        unsigned int FileSize = ftell(File);
        fseek(File, 0, SEEK_SET);

        FileContents = (char *)malloc(FileSize+1);
        if(FileContents)
        {
            fread(FileContents, 1, FileSize, File);
            FileContents[FileSize] = 0;
        }
        
        fclose(File);
    }

    NSBuildParseConfig(FileContents, &Config);
    free(FileContents);

    return (Config);
}

static void
NSBuildConfigUnload(NSBuildConfig *Config)
{
    free(Config->DynamicCodeFileName);
    Config->DynamicCodeFileName = 0;
        
    free(Config->EntryFileName);
    Config->EntryFileName = 0;
    
    free(Config->CompilerBackend);
    Config->CompilerBackend = 0;
    
    free(Config->CompilerArguments);
    Config->CompilerArguments = 0;
    
    free(Config->LinkerArguments);
    Config->LinkerArguments = 0;
}
