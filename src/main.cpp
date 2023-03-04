#include <iostream>
#include <angelscript.h>
#include <scriptbuilder/scriptbuilder.h>
#include <scriptstdstring/scriptstdstring.h>

#include <cstdio>
#include <cassert>

#include "json_object.hpp"

#ifdef __APPLE__
    #include <sys/time.h>
    #include <termios.h>
    #include <unistd.h>
#endif

typedef unsigned int DWORD;

// proto functions

void configEngine(asIScriptEngine *&engine);
int compileScript(asIScriptEngine *&engine);
void lineCallBack(asIScriptContext *&ctx, DWORD *timeOut);


DWORD timeGetTime()
{
    timeval time;
    gettimeofday(&time, NULL);
    // miliseconds
    return (time.tv_sec*1000) + (time.tv_usec/1000);
}


void messageCallback(const asSMessageInfo *msg, void *param)
{
    const char *type = "ERR ";
    if(msg->type == asMSGTYPE_WARNING)
    {
        type = "WARN";
    }
    else if(msg->type == asMSGTYPE_INFORMATION)
    {   
        type = "INFO";
    }
    printf("%s (%d, %d) : %s : %s\n", msg->section, msg->row, msg->col, type, msg->message);
}

// Function implementation with native calling convention
void printString(std::string &str)
{
	std::cout << str;
}


// Function implementation with generic script interface
void printString_Generic(asIScriptGeneric *gen)
{
	std::string *str = (std::string*)gen->GetArgAddress(0);
	std::cout << *str;
}

// Function implementation with generic script interface
void printInt_Generic(asIScriptGeneric *gen)
{
	int *number = (int*)gen->GetArgAddress(0);
	std::cout << *number;
}

int main()
{
    // create the engine
    asIScriptEngine *engine = asCreateScriptEngine();

    if(!engine)
    {
        std::cout << "Failed to create script engine.\n";
    }

    //set message callback to get info back
    int r = engine->SetMessageCallback(asFUNCTION(messageCallback), 0, asCALL_CDECL);
    assert(r >= 0);

   configEngine(engine);

   r = compileScript(engine);
   if(r < 0)
   {
        engine->Release();
        return -1;
   }

   // engine context, executes scripts
   asIScriptContext *ctx = engine->CreateContext();
   if(ctx == 0)
   {
        std::cout << "Failed to create the context\n";
        engine->Release();
        return -1;
   }

    // stop infinite loops
    DWORD timeOut;
    r = ctx->SetLineCallback(asFUNCTION(lineCallBack), &timeOut, asCALL_CDECL);
    if(r < 0)
    {
        std::cout << "Failed to set line callback function\n";
        ctx->Release();
        engine->Release();
        return -1;
    }

    //find function, get pointer to it
    asIScriptFunction* func = engine->GetModule(0)->GetFunctionByDecl("void main()");
    if(!func)
    {
        std::cout << "The function was not found.\n";
        ctx->Release();
        engine->Release();
        return -1;
    }

    // prepare the script context with the function we want to exec.
    r = ctx->Prepare(func);
    if(r < 0)
    {
        std::cout << "Failed to prepare the context";
        ctx->Release();
        engine->Release();
        return -1;
    }

    //Set timeout for script execution
    timeOut = timeGetTime() + 1000;

    //execute function

    std::cout << "Execting the script." << std::endl;

    r = ctx->Execute();

    if ( r != asEXECUTION_FINISHED)
    {
        if(r == asEXECUTION_ABORTED)
        {
            std::cout << "Script was aborted.\n";
        }
        else if(r == asEXECUTION_EXCEPTION)
        {
            std::cout << "The script ended with an execption\n.";

            // write script info to find execption
            asIScriptFunction *funcEx = ctx->GetExceptionFunction();
            std::cout << "Func: " << funcEx->GetDeclaration() << "\n";
            std::cout << "modl: " << funcEx->GetModuleName() << "\n";
            std::cout << "sect: " << funcEx->GetScriptSectionName() << "\n";
            std::cout << "line: " << ctx->GetExceptionLineNumber() << "\n";
            std::cout << "desc: " << ctx->GetExceptionString() << "\n";
        }
        else
        {
            std::cout << "Script ended for some unknown reason (" << r << ").\n";
        }
    }
    else
    {
        std::cout << "Script finished\n"; 
    }

    // release context, we no longer need it.
    ctx->Release();

    // shut down the engine
    engine->ShutDownAndRelease();

    return 0;

}

int compileScript(asIScriptEngine *&engine)
{
    int r;

    // The builder is a helper class that will load the script file, 
	// search for #include directives, and load any included files as 
	// well.
    CScriptBuilder builder;

    // build script
    r = builder.StartNewModule(engine, 0);
    if(r < 0)
    {
        std::cout << "Failed to start new module\n";
        return r;
    }

    //file
    r = builder.AddSectionFromFile("/Users/ellisnielsen/Documents/c++/angel/scripts/hello.as");
    if(r < 0)
    {
        std::cout << "Failed to add script file\n";
        return r;
    }

    //build mod
    r = builder.BuildModule();
    if(r < 0)
    {
        std::cout << "Failed to build module\n";
        return r;
    }

    return 0;
}

void configEngine(asIScriptEngine *&engine)
{
    //return code
    int r;

    // register string type
    // Look at the implementation for this function for more information  
	// on how to register a custom string type, and other object types.
    RegisterStdString(engine);
    register_json(engine);

    if(!strstr(asGetLibraryOptions(), "AS_MAX_PORTABILITY"))
    {
        std::cout << " not AS_MAX_PORTABILITY\n";
        r = engine->RegisterGlobalFunction("void print(string &in)", asFUNCTION(printString), asCALL_CDECL);
        assert(r>=0);
    }
    else
    {
        r = engine->SetDefaultNamespace("sqf"); assert( r >= 0 );
        // Notice how the registration is almost identical to the above. 
		r = engine->RegisterGlobalFunction("void print(string &in)", asFUNCTION(printString_Generic), asCALL_GENERIC); assert( r >= 0 );
        r = engine->RegisterGlobalFunction("void print(int &in)", asFUNCTION(printInt_Generic), asCALL_GENERIC); assert( r >= 0 );
    }
}

void lineCallBack(asIScriptContext *&ctx, DWORD *timeOut)
{
    if(*timeOut < timeGetTime())
    {
        ctx->Abort();
    }
}