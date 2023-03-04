#include "json_object.hpp"

// We'll use the generic interface for the factories as we need the engine pointer
static void json_factory_generic(asIScriptGeneric *gen)
{
    asIScriptEngine *engine = gen->GetEngine();

    // yes two-star programming
    *static_cast<json_object**>(gen->GetAddressOfReturnLocation()) = new json_object(engine);
}


json_object::json_object(asIScriptEngine *&engine)
{
    std::cout << "Json Object created!\n";
    refCounter = 1;

    gcFlag = false;

    // Notify the garbage collector of this object
    engine->NotifyGarbageCollectorOfNewObject(this, engine->GetTypeInfoByName("any"));
}

int json_object::add_ref() const
{
    // Increase counter and clear flag set by GC
    gcFlag = false;
    return asAtomicInc(refCounter);
}

int json_object::release() const
{
    // Decrease the ref counter
    gcFlag = false;
    if( asAtomicDec(refCounter) == 0 )
    {
        // Delete this object as no more references to it exists
        delete this;
        return 0;
    }

    return refCounter;
}

int json_object::get_ref_count()
{
    return refCounter;
}

bool json_object::get_flag()
{
    return gcFlag;
}

void json_object::set_flag() {
    gcFlag = true;
}


void json_object::push_back(nlohmann::json &value)
{
}

void json_object::push_back(nlohmann::json &&value)
{
}

void json_object::push_back(const nlohmann::json &&value) {

}

json_object::~json_object() {

}


void register_json_native(asIScriptEngine *&engine)
{
    int retCode;
    retCode = engine->RegisterObjectType("json", sizeof(json_object), asOBJ_REF | asOBJ_GC); assert(retCode >= 0);

    // We'll use the generic interface for the constructor as we need the engine pointer
    retCode = engine->RegisterObjectBehaviour("json", asBEHAVE_FACTORY, "json@ f()", asFUNCTION(json_factory_generic), asCALL_GENERIC); assert( retCode >= 0 );

    retCode = engine->RegisterObjectBehaviour("json", asBEHAVE_ADDREF, "void f()", asMETHOD(json_object,add_ref), asCALL_THISCALL); assert( retCode >= 0 );
    retCode = engine->RegisterObjectBehaviour("json", asBEHAVE_RELEASE, "void f()", asMETHOD(json_object,release), asCALL_THISCALL); assert( retCode >= 0 );

    // gc stuff
    retCode = engine->RegisterObjectBehaviour("json", asBEHAVE_GETREFCOUNT, "int f()", asMETHOD(json_object,get_ref_count), asCALL_THISCALL); assert( retCode >= 0 );
    retCode = engine->RegisterObjectBehaviour("json", asBEHAVE_SETGCFLAG, "void f()", asMETHOD(json_object,set_flag), asCALL_THISCALL); assert( retCode >= 0 );
    retCode = engine->RegisterObjectBehaviour("json", asBEHAVE_GETGCFLAG, "bool f()", asMETHOD(json_object,get_flag), asCALL_THISCALL); assert( retCode >= 0 );
}

