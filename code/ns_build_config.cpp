/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Oskar Mendel $
   $Notice: (C) Copyright 2020 by Nullsson, Inc. All Rights Reserved. $
   ======================================================================== */

struct NSBuildConfig
{
    char *DynamicCodeEntryFileName;
    char *DynamicCodeOutputFileName;
    char *DynamicCodeCompilerBackend;
    char *DynamicCodeCompilerFlags;
    char *DynamicCodeLinkerFlags;
    char *EntryFileName;
    char *OutputFileName;
    char *CompilerBackend;
    char *CompilerFlags;
    char *LinkerFlags;
    char *BuildOutputDirectory;
    bool HasBuildDirectory;
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
            char *ValueCopy = static_cast<char *>(malloc(StringSize));
            strcpy(ValueCopy, Value);

            // NOTE(Oskar): Remove carraige return from string if exist.
            char *CarriageReturn = strchr(ValueCopy, '\r');
            if (CarriageReturn != NULL)
            {
                *CarriageReturn = '\0';
            }

            if (StringsAreEqual(Key, "DYNAMICCODEENTRYFILENAME"))
            {
                Config->DynamicCodeEntryFileName = ValueCopy;
            }
            else if (StringsAreEqual(Key, "DYNAMICCODEOUTPUTFILENAME"))
            {
                Config->DynamicCodeOutputFileName = ValueCopy;
            }
            else if (StringsAreEqual(Key, "DYNAMICCODECOMPILERBACKEND"))
            {
                Config->DynamicCodeCompilerBackend = ValueCopy;
            }
            else if (StringsAreEqual(Key, "DYNAMICCODECOMPILERFLAGS"))
            {
                Config->DynamicCodeCompilerFlags = ValueCopy;
            }
            else if (StringsAreEqual(Key, "DYNAMICCODELINKERFLAGS"))
            {
                Config->DynamicCodeLinkerFlags = ValueCopy;
            }
            else if (StringsAreEqual(Key, "ENTRYFILENAME"))
            {
                Config->EntryFileName = ValueCopy;
            }
            else if (StringsAreEqual(Key, "OUTPUTFILENAME"))
            {
                Config->OutputFileName = ValueCopy;
            }
            else if (StringsAreEqual(Key, "COMPILERBACKEND"))
            {
                Config->CompilerBackend = ValueCopy;
            }
            else if (StringsAreEqual(Key, "COMPILERFLAGS"))
            {
                Config->CompilerFlags = ValueCopy;
            }
            else if (StringsAreEqual(Key, "LINKERFLAGS"))
            {
                Config->LinkerFlags = ValueCopy;
            }
            else if (StringsAreEqual(Key, "BUILDOUTPUTDIRECTORY"))
            {
                Config->BuildOutputDirectory = ValueCopy;
                if (StringLength(Config->BuildOutputDirectory) > 0)
                {
                    Config->HasBuildDirectory = true;
                }
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
    Config.HasBuildDirectory = false;

    char *FileContents = 0;
    FILE *File = fopen(ConfigPath, "rb");
    if(File)
    {
        fseek(File, 0, SEEK_END);
        unsigned int FileSize = ftell(File);
        fseek(File, 0, SEEK_SET);

        FileContents = static_cast<char *>(malloc(FileSize+1));
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
    free(Config->DynamicCodeEntryFileName);
    Config->DynamicCodeEntryFileName = 0;

    free(Config->DynamicCodeOutputFileName);
    Config->DynamicCodeOutputFileName = 0;

    free(Config->DynamicCodeCompilerBackend);
    Config->DynamicCodeCompilerBackend = 0;

    free(Config->DynamicCodeCompilerFlags);
    Config->DynamicCodeCompilerFlags = 0;

    free(Config->DynamicCodeLinkerFlags);
    Config->DynamicCodeLinkerFlags = 0;
    
    free(Config->EntryFileName);
    Config->EntryFileName = 0;

    free(Config->OutputFileName);
    Config->OutputFileName = 0;
    
    free(Config->CompilerBackend);
    Config->CompilerBackend = 0;
    
    free(Config->CompilerFlags);
    Config->CompilerFlags = 0;
    
    free(Config->LinkerFlags);
    Config->LinkerFlags = 0;

    free(Config->BuildOutputDirectory);
    Config->BuildOutputDirectory = 0;
}
